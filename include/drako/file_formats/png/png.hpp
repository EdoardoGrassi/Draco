#pragma once
#ifndef DRAKO_PNG_HPP
#define DRAKO_PNG_HPP

#include <stdexcept>
#include <vector>

namespace drako::formats::png
{
    /// @brief Color encoded as RGB tuple.
    struct rgb
    {
        std::uint8_t r, g, b;
    };

    /// @brief Exception emitted on parsing errors.
    class ParserError : public std::runtime_error
    {
    public:
        explicit ParserError(const char* msg)
            : std::runtime_error(msg) {}
    };


    struct image
    {
        std::vector<rgb> palette;
    };


    class parser
    {
    };
} // namespace drako::formats::png

#endif // !DRAKO_PNG_HPP