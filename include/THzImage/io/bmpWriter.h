#ifndef THZ_IMAGE_IO_BMPWRITER_H
#define THZ_IMAGE_IO_BMPWRITER_H

#include "THzImage/common/iImageWriter.h"
#include "THzImage/common/pixel.h"

#include <string_view>

namespace Terrahertz::BMP {

/// @brief Writes an image to a file using the BitMap format.
class Writer : public IImageWriter<BGRAPixel>
{
public:
    /// @brief Initializes a nwe BMPWriter.
    ///
    /// @param filepath The path to write the BMP-File to.
    /// @param transparency True if transparency should be saved, false otherwise.
    Writer(std::string_view const filepath, bool const transparency = true) noexcept;

    /// @copydoc IImageWriter::init
    bool init() noexcept override;

    /// @copydoc IImageWriter::write
    bool write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept override;

    /// @copydoc IImageWriter::deinit
    void deinit() noexcept override;

private:
    /// @brief The path to write the BMP-File to.
    std::string_view const _filepath;

    /// @brief The bit count of the image (24 or 32).
    uint8_t const _bitCount;
};

} // namespace Terrahertz::BMP

#endif // !THZ_IMAGE_IO_BMPWRITER_H
