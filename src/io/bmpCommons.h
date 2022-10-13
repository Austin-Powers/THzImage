#ifndef THZ_IMAGE_IO_BMPCOMMONS_H
#define THZ_IMAGE_IO_BMPCOMMONS_H

#include <cstdint>

namespace Terrahertz::BMP {

#pragma pack(1)

/// @brief Replica of the BITMAPFILEHEADER from windows.h.
struct FileHeader
{
    /// @brief The value of the magic bytes of the file header.
    static constexpr std::uint16_t MagicBytes{0x4D42};

    /// @brief The magic bytes of the file header.
    std::uint16_t magic{MagicBytes};

    /// @brief The size of the entire BMP-file.
    std::uint32_t size{};

    /// @brief Reserved bytes.
    std::uint32_t reserved{};

    /// @brief The offset of the image data from the start of the file [bytes].
    std::uint32_t offBits{54U};
};

static_assert(sizeof(FileHeader) == 14U, "FileHeader has the wrong size.");

/// @brief Replica of the BITMAPINFOHEADER from windows.h.
struct InfoHeader
{
    /// @brief The size of the InfoHeader structure [bytes].
    std::uint32_t size{40U};

    /// @brief The width of the image [pxl].
    std::int32_t width{};

    /// @brief The height of the image [pxl], negative value means the image is build top-down.
    std::int32_t height{};

    /// @brief The amount of planes of the image, BMP only supports 1.
    std::uint16_t planes{1U};

    /// @brief The bits used to store one pixel.
    std::uint16_t bitCount{24U};

    /// @brief The compression of the image, 0 for uncompressed.
    std::uint32_t compression{0U};

    /// @brief The size of the image data [bytes].
    std::uint32_t sizeImage{};

    /// @brief Horizontal resolution of the target device [pxl/m].
    std::int32_t xPelsPerMeter{0};

    /// @brief Vertical resolution of the target device [pxl/m].
    std::int32_t yPelsPerMeter{0};

    /// @brief Colortable used, unused.
    std::uint32_t clrUsed{0U};

    /// @brief Colortable important, unused.
    std::uint32_t clrImportant{0U};
};

static_assert(sizeof(InfoHeader) == 40U, "InfoHeader has the wrong size.");

/// @brief The combined BMP-File header.
struct Header
{
    /// @brief The FileHeader.
    FileHeader fileHeader{};

    /// @brief The InfoHeader.
    InfoHeader infoHeader{};
};

static_assert(sizeof(Header) == 54U, "Header has the wrong size.");

#pragma pack()

} // namespace Terrahertz::BMP

#endif // !THZ_IMAGE_IO_BMPCOMMONS_H