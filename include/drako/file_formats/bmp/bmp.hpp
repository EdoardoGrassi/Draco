#pragma once
#ifndef DRAKO_FILE_FORMATS_BMP_HPP
#define DRAKO_FILE_FORMATS_BMP_HPP

#include <cstddef>
#include <optional>
#include <span>
#include <stdexcept>
#include <vector>

namespace drako::file_formats::bmp
{
    [[nodiscard]] constexpr std::uint16_t from_little_endian_bytes(std::array<std::byte, 2> bytes) noexcept
    {
        return (std::to_integer<std::uint16_t>(bytes[0])) |
               (std::to_integer<std::uint16_t>(bytes[1]) << 8);
    }

    [[nodiscard]] constexpr std::uint32_t from_little_endian_bytes(std::array<std::byte, 4> bytes) noexcept
    {
        return (std::to_integer<std::uint32_t>(bytes[0])) |
               (std::to_integer<std::uint32_t>(bytes[1]) << 8) |
               (std::to_integer<std::uint32_t>(bytes[2]) << 16) |
               (std::to_integer<std::uint32_t>(bytes[3]) << 24);
    }

    struct rgba
    {
        std::uint8_t r, g, b, a;
    };

    struct palette_t
    {
        std::vector<rgba> table;
    };


    class parser_error : std::runtime_error
    {
    public:
        explicit parser_error(const char* msg)
            : std::runtime_error{ msg }
        {
        }
    };


    class parse_result
    {
    public:
        explicit parse_result();

        [[nodiscard]] std::size_t width() const noexcept;
        [[nodiscard]] std::size_t height() const noexcept;

        [[nodiscard]] std::optional<palette_t> palette() const noexcept;

    private:
        std::optional<palette_t> _palette;
        std::size_t              _width;
        std::size_t              _height;
    };



    [[nodiscard]] parse_result parse(const std::span<std::byte> data);

} // namespace drako::file_formats::bmp

#endif // !DRAKO_FILE_FORMATS_BMP_HPP