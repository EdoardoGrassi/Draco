#pragma once
#ifndef DRAKO_VULKAN_STAGING_ENGINE_HPP
#define DRAKO_VULKAN_STAGING_ENGINE_HPP

#include "drako/graphics/vulkan_queue.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/graphics/vulkan_utils.hpp"

#include <cassert>
#include <functional>
#include <span>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace drako::vulkan
{
    struct memory_transfer
    {
        std::span<const std::byte> source;
        vk::Buffer                 destination;
        std::size_t                offset;
    };


    class stack_allocator
    {
    public:
        stack_allocator() = default;

        stack_allocator(std::byte* memory, std::size_t bytes) noexcept
            : _memory{ memory }, _bytes{ bytes }
        {
            assert(memory);
            assert(bytes > 0);

            assert(used_bytes() == 0);
            assert(free_bytes() == bytes);
        }

        [[nodiscard]] std::byte* allocate(std::size_t bytes) noexcept
        {
            if (const auto end = _memory + _bytes;
                std::distance(_free, end) >= bytes)
            {
                auto ptr = _free;
                _free += bytes;
                return ptr;
            }
            else
                return nullptr;
        }

        // Amount of bytes currently allocated.
        [[nodiscard]] std::size_t used_bytes() const noexcept
        {
            return std::distance(_memory, _free);
        }

        // Amount of bytes currently not allocated.
        [[nodiscard]] std::size_t free_bytes() const noexcept
        {
            return std::distance(_free, _memory + _bytes);
        }

        // Convert an allocated pointer into an offset in the original memory buffer.
        [[nodiscard]] std::size_t offset(std::byte* allocation) const noexcept
        {
            assert(allocation >= _memory);
            return std::distance(_memory, allocation);
        }

        void reset() noexcept
        {
            _free = _memory;
        }

    private:
        std::size_t _bytes;  // managed bytes
        std::byte*  _memory; // managed memory
        std::byte*  _free;   // first free byte
    };


    class block_allocator
    {
    public:
        block_allocator() = default;

        block_allocator(std::byte* memory, std::size_t bytes, std::size_t bsize)
            : _memory{ memory }, _bbytes{ bytes }
        {
            assert(memory);
            assert(bytes > 0);
            assert(bsize > 0);

            const auto blockcount = bytes / bsize;
            _blocks.resize(blockcount);
            for (auto i = 0; i < blockcount; ++i)
                _blocks[i] = i;
        }

        [[nodiscard]] std::byte* allocate();

        void deallocate(std::byte*);

        [[nodiscard]] std::size_t free_blocks() noexcept { return std::size(_blocks); }

    private:
        std::vector<std::uint16_t> _blocks; // free blocks
        const std::size_t          _bbytes; // byte size of a single block
        std::byte* const           _memory; // managed memory
    };


    const std::size_t BLOCK_SIZE = 1024;

    class staging_engine
    {
    public:
        using transfer_completed_callback = std::function<void()>;

        staging_engine(
            const Context& ctx, std::size_t bytes, const transfer_queue& queue);

        staging_engine(const staging_engine&) = delete;
        staging_engine& operator=(const staging_engine&) = delete;

        ~staging_engine() noexcept;

        /// @brief Submit a memory transfer operation.
        void submit(const memory_transfer& op) noexcept;

        /// @brief Submit a memory transfer operation.
        /// @param[in] op Details of the operation.
        /// @param[in] c Callback invoked on operation completition.
        ///
        /// @remark Both source and destination buffer must remain valid
        ///     for the duration of the operation.
        ///
        void submit(const memory_transfer& op, transfer_completed_callback c) noexcept;

        void update() noexcept;

    private:
        const vk::PhysicalDevice _pdevice;
        const vk::Device         _ldevice;
        const transfer_queue     _queue;
        vk::UniqueBuffer         _staging_buffer;
        vk::UniqueDeviceMemory   _device_memory;
        std::span<std::byte>     _host_memory;
        vk::MemoryPropertyFlags  _staging_memory_specs;

        stack_allocator _allocator;

        vk::UniqueCommandPool   _transfer_cmd_pool;
        vk::UniqueCommandBuffer _transfer_cmd_buffer;
        vk::Queue               _transfer_queue;
        vk::UniqueFence         _transfer_completed;

        std::vector<memory_transfer>             _wait_for_submit_transfers;
        std::vector<transfer_completed_callback> _wait_for_submit_callbacks;
        std::vector<transfer_completed_callback> _wait_for_completition_callbacks;

        void _submit_batch() noexcept;
    };

    staging_engine::staging_engine(
        const Context& ctx, size_t bytes, const transfer_queue& queue)
        : _pdevice(ctx.physical_device)
        , _ldevice(ctx.logical_device.get())
    {
        assert(bytes > 0);

        _wait_for_submit_transfers.reserve(32);
        _wait_for_submit_callbacks.reserve(32);
        _wait_for_completition_callbacks.reserve(32);

        {
            const vk::BufferCreateInfo staging_buffer_info{
                vk::BufferCreateFlagBits{},
                vk::DeviceSize{ bytes },
                vk::BufferUsageFlagBits::eTransferSrc,
                vk::SharingMode::eConcurrent,
                0, nullptr /* Not needed for exclusive ownership */
            };
            _staging_buffer = _ldevice.createBufferUnique(staging_buffer_info);
        }

        {
            const auto mem_loader_specs = vk::MemoryPropertyFlagBits::eHostVisible |
                                          vk::MemoryPropertyFlagBits::eHostCoherent;
            const auto mem_buffer_specs = _ldevice.getBufferMemoryRequirements(_staging_buffer.get());
            const auto properties       = _pdevice.getMemoryProperties();

            uint32_t i = 0;
            for (; i < properties.memoryTypeCount; ++i)
            {
                if ((mem_buffer_specs.memoryTypeBits & (1 << i)) &&
                    (properties.memoryTypes[i].propertyFlags & mem_loader_specs))
                {
                    _staging_memory_specs = properties.memoryTypes[i].propertyFlags;
                    break;
                }
            }
            const vk::MemoryAllocateInfo alloc_info{ vk::DeviceSize{ bytes }, i };
            _device_memory = _ldevice.allocateMemoryUnique(alloc_info);
            _ldevice.bindBufferMemory(_staging_buffer.get(), _device_memory.get(), vk::DeviceSize{ 0 });

            const auto ptr = _ldevice.mapMemory(_device_memory.get(), 0, VK_WHOLE_SIZE, {});
            _allocator     = stack_allocator{ static_cast<std::byte*>(ptr), bytes };
        }

        {
            const vk::CommandPoolCreateInfo command_pool_create_info{
                vk::CommandPoolCreateFlagBits::eTransient,
                _queue.family
            };
            _transfer_cmd_pool = _ldevice.createCommandPoolUnique(command_pool_create_info);
        }

        {
            const vk::CommandBufferAllocateInfo command_buffer_alloc_info{
                _transfer_cmd_pool.get(),
                vk::CommandBufferLevel::ePrimary,
                1 // command buffers count
            };
            auto buffers = _ldevice.allocateCommandBuffersUnique(command_buffer_alloc_info);
            assert(std::size(buffers) == 1);
            _transfer_cmd_buffer = std::move(buffers[0]);
        }

        {
            const vk::FenceCreateInfo fence_create_info{}; // init as unsignaled
            _transfer_completed = _ldevice.createFenceUnique(fence_create_info);
        }
    }

    staging_engine::~staging_engine() noexcept
    {
        _ldevice.unmapMemory(_device_memory.get());
    }

    void staging_engine::submit(const memory_transfer& op) noexcept
    {
        _wait_for_submit_transfers.push_back(op);
    }

    void staging_engine::submit(const memory_transfer& op, transfer_completed_callback cb) noexcept
    {
        assert(cb != nullptr);

        _wait_for_submit_transfers.push_back(op);
        _wait_for_submit_callbacks.push_back(cb);
    }

    void staging_engine::update() noexcept
    {
        switch (const auto r = _ldevice.getFenceStatus(_transfer_completed.get()); r)
        {
            case vk::Result::eSuccess:
                break;
            case vk::Result::eNotReady: // queue is still busy
                return;
            default:
                log_and_exit("[Vulkan] Unexpected result " + to_string(r));
        }

        // previous batch has finished execution
        for (const auto& callback : _wait_for_completition_callbacks)
            std::invoke(callback);
        _wait_for_completition_callbacks.clear();


        _transfer_cmd_buffer.get().begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr });


        _allocator.reset();
        for (auto i = 0; i < std::size(_wait_for_submit_transfers); ++i)
        {
            const auto& t       = _wait_for_submit_transfers[i];
            const auto  staging = _allocator.allocate(t.source.size_bytes());
            if (staging == nullptr)
                break;

            std::memcpy(staging, t.source.data(), t.source.size_bytes());
            const auto offset = _allocator.offset(staging);

            vk::BufferCopy region{};
            region.size      = t.source.size_bytes();
            region.srcOffset = offset;
            region.dstOffset = t.offset;
            _transfer_cmd_buffer.get().copyBuffer(_staging_buffer.get(), t.destination, 1, &region);
        }

        _transfer_cmd_buffer.get().end();

        const auto flush_not_required = vk::MemoryPropertyFlagBits::eHostCoherent |
                                        vk::MemoryPropertyFlagBits::eDeviceCoherentAMD;
        if (!(_staging_memory_specs & flush_not_required))
        { // request explicit flush
            const vk::MappedMemoryRange range{
                _device_memory.get(),
                0,
                vk::DeviceSize{ _allocator.used_bytes() }
            };
            _ldevice.flushMappedMemoryRanges(1, &range);
        }

        _ldevice.resetFences(1, &_transfer_completed.get());
        {
            vk::SubmitInfo job{};
            job.commandBufferCount = 1;
            job.pCommandBuffers    = &_transfer_cmd_buffer.get();
            _transfer_queue.submit(1, &job, _transfer_completed.get());
        }
    }
} // namespace drako::vulkan

#endif // !DRAKO_VULKAN_STAGING_ALLOCATOR_HPP