#pragma once
#ifndef DRAKO_TYPED_HANDLE_HPP
#define DRAKO_TYPED_HANDLE_HPP

#include <cstdint>
#include <type_traits>

namespace drako
{
    template <typename T, typename Int>
    requires std::is_unsigned_v<Int> class basic_typed_handle
    {
    public:
        using _this = basic_typed_handle;

        explicit constexpr basic_typed_handle() noexcept = default;

        explicit constexpr basic_typed_handle(const Int key) noexcept
            : _key{ key } {}

        constexpr basic_typed_handle(const _this& other) noexcept
            : _key{ other._key } {}

        constexpr _this& operator=(const _this& other) noexcept
        {
            _key = other._key;
            return *this;
        }

        [[nodiscard]] constexpr friend bool operator==(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator!=(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator<(const _this, const _this) noexcept  = default;
        [[nodiscard]] constexpr friend bool operator>(const _this, const _this) noexcept  = default;
        [[nodiscard]] constexpr friend bool operator<=(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator>=(const _this, const _this) noexcept = default;

        //[[nodiscard]] constexpr bool operator==(const basic_typed_handle& rhs) const noexcept { return _key == rhs._key; }
        //[[nodiscard]] constexpr bool operator!=(const basic_typed_handle& rhs) const noexcept { return _key != rhs._key; }
        //[[nodiscard]] constexpr bool operator>(const basic_typed_handle& rhs) const noexcept { return _key > rhs._key; }
        //[[nodiscard]] constexpr bool operator<(const basic_typed_handle& rhs) const noexcept { return _key < rhs._key; }
        //[[nodiscard]] constexpr bool operator>=(const basic_typed_handle& rhs) const noexcept { return _key >= rhs._key; }
        //[[nodiscard]] constexpr bool operator<=(const basic_typed_handle& rhs) const noexcept { return _key <= rhs._key; }

        [[nodiscard]] constexpr operator bool() const noexcept { return _key != 0; }

    private:
        Int _key = 0;
    };

    template <typename T>
    using handle = basic_typed_handle<T, std::uint32_t>;


    template <typename T, typename Int>
    requires std::is_unsigned_v<Int> class basic_typed_id
    {
    public:
        using _this = basic_typed_id;

        explicit constexpr basic_typed_id() noexcept = default;

        explicit constexpr basic_typed_id(const Int key) noexcept
            : _key{ key } {}

        constexpr basic_typed_id(const _this& other) noexcept
            : _key{ other._key } {}

        constexpr _this& operator=(const _this& other) noexcept
        {
            _key = other._key;
            return *this;
        }

        [[nodiscard]] constexpr friend bool operator==(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator!=(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator<(const _this, const _this) noexcept  = default;
        [[nodiscard]] constexpr friend bool operator>(const _this, const _this) noexcept  = default;
        [[nodiscard]] constexpr friend bool operator<=(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator>=(const _this, const _this) noexcept = default;

        //[[nodiscard]] constexpr bool operator==(const basic_typed_id rhs) const noexcept { return _key == rhs._key; }
        //[[nodiscard]] constexpr bool operator!=(const basic_typed_id rhs) const noexcept { return _key != rhs._key; }
        //[[nodiscard]] constexpr bool operator>(const basic_typed_id rhs) const noexcept { return _key > rhs._key; }
        //[[nodiscard]] constexpr bool operator<(const basic_typed_id rhs) const noexcept { return _key < rhs._key; }
        //[[nodiscard]] constexpr bool operator>=(const basic_typed_id rhs) const noexcept { return _key >= rhs._key; }
        //[[nodiscard]] constexpr bool operator<=(const basic_typed_id rhs) const noexcept { return _key <= rhs._key; }

        [[nodiscard]] constexpr operator bool() const noexcept { return _key != 0; }

    private:
        Int _key = 0;
    };

    template <typename T>
    using id = basic_typed_id<T, std::uint32_t>;

} // namespace drako

#endif // !DRAKO_TYPED_HANDLE_HPP