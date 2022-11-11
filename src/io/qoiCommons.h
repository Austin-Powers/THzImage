#ifndef THZ_IMAGE_IO_QOICOMMONS_H
#define THZ_IMAGE_IO_QOICOMMONS_H

namespace Terrahertz::QOI {

#pragma pack(1) // otherwise the structs would turn out too large
/// @brief The header of the QOI file format.
struct Header
{
    /// @brief The magic bytes 'qoif'.
    char magic[4];

    /// @brief The width of the image [pxl].
    uint32_t width;

    /// @brief The height of the image [pxl].
    uint32_t height;

    /// @brief The color channels 3 = 'rgb' 4 = 'rgba'
    ///
    /// @remarks This field is purely informative.
    uint8_t channels;

    /// @brief 0 = sRGB with linear alpha, 1 = all channels linear.
    ///
    /// @remarks This field is purely informative.
    uint8_t colorspace;
};
static_assert(sizeof(Header) == 14U, "Header too large");
#pragma pack()

/// @brief Code for index block: 0b00xxxxxx
constexpr uint8_t OpIndex{0x00U};

/// @brief Code for diff block:  0b01xxxxxx
constexpr uint8_t OpDiff{0x40U};

/// @brief Code for luma block:  0b10xxxxxx
constexpr uint8_t OpLuma{0x80U};

/// @brief Code for run block:   0b11xxxxxx
constexpr uint8_t OpRun{0xC0U};

/// @brief Code for RGB block:   0x11111110
constexpr uint8_t OpRGB{0xFEU};

/// @brief Code for RGBA block:  0x11111111
constexpr uint8_t OpRGBA{0xFFU};

/// @brief Mask for the 2 most significant bits: 0x11000000
constexpr uint8_t Mask2{0xC0};

/// @brief The color table for encoding and decoding.
using ColorTable = std::array<BGRAPixel, 64U>;

/// @brief  Calculates the hash of a Pixel.
///
/// @param red The red value of the pixel.
/// @param green The green value of the pixel.
/// @param blue The blue value of the pixel.
/// @param alpha The alpha value of the pixel.
/// @return The hash value.
constexpr uint8_t pixelHash(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) noexcept
{
    // % 64 == & 0b111111
    return (red * 3 + green * 5 + blue * 7 + alpha * 11) & 0b111111;
}

/// @brief Calculates the hash of a Pixel.
///
/// @param pixel The pixel to calculate the hash for.
/// @return The hash value.
constexpr uint8_t pixelHash(BGRAPixel const &pixel) noexcept
{
    return pixelHash(pixel.red, pixel.green, pixel.blue, pixel.alpha);
}

} // namespace Terrahertz::QOI

#endif // !THZ_IMAGE_IO_QOICOMMONS_H
