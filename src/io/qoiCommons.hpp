#ifndef THZ_IMAGE_IO_QOICOMMONS_HPP
#define THZ_IMAGE_IO_QOICOMMONS_HPP

#include <cstdint>

namespace Terrahertz::QOI {

#pragma pack(1) // otherwise the structs would turn out too large
/// @brief The header of the QOI file format.
struct Header
{
    /// @brief The value of the magic bytes of the file header.
    static constexpr std::uint32_t MagicBytes{0x66696F71};

    /// @brief The magic bytes 'qoif'.
    std::uint32_t magic{};

    /// @brief The width of the image [pxl].
    std::uint32_t width;

    /// @brief The height of the image [pxl].
    std::uint32_t height;

    /// @brief The color channels 3 = 'rgb' 4 = 'rgba'
    ///
    /// @remarks This field is purely informative.
    std::uint8_t channels;

    /// @brief 0 = sRGB with linear alpha, 1 = all channels linear.
    ///
    /// @remarks This field is purely informative.
    std::uint8_t colorspace;
};
static_assert(sizeof(Header) == 14U, "Header too large");
#pragma pack()

/// @brief Code for index block: 0b00xxxxxx
constexpr std::uint8_t OpIndex{0x00U};

/// @brief Code for diff block:  0b01xxxxxx
constexpr std::uint8_t OpDiff{0x40U};

/// @brief Code for luma block:  0b10xxxxxx
constexpr std::uint8_t OpLuma{0x80U};

/// @brief Code for run block:   0b11xxxxxx
constexpr std::uint8_t OpRun{0xC0U};

/// @brief Code for RGB block:   0x11111110
constexpr std::uint8_t OpRGB{0xFEU};

/// @brief Code for RGBA block:  0x11111111
constexpr std::uint8_t OpRGBA{0xFFU};

/// @brief Mask for the 2 most significant bits: 0x11000000
constexpr std::uint8_t Mask2{0xC0U};

/// @brief The color table for encoding and decoding.
using ColorTable = std::array<BGRAPixel, 64U>;

/// @brief  Calculates the hash of a Pixel.
///
/// @param red The red value of the pixel.
/// @param green The green value of the pixel.
/// @param blue The blue value of the pixel.
/// @param alpha The alpha value of the pixel.
/// @return The hash value.
constexpr std::uint8_t
pixelHash(std::uint8_t const red, std::uint8_t const green, std::uint8_t const blue, std::uint8_t const alpha) noexcept
{
    // % 64 == & 0b111111
    return (red * 3U + green * 5U + blue * 7U + alpha * 11U) & 0b111111U;
}

/// @brief Calculates the hash of a Pixel.
///
/// @param pixel The pixel to calculate the hash for.
/// @return The hash value.
constexpr std::uint8_t pixelHash(BGRAPixel const &pixel) noexcept
{
    return pixelHash(pixel.red, pixel.green, pixel.blue, pixel.alpha);
}

} // namespace Terrahertz::QOI

#endif // !THZ_IMAGE_IO_QOICOMMONS_HPP
