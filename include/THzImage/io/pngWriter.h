#ifndef THZ_IMAGE_IO_PNGWRITER_H
#define THZ_IMAGE_IO_PNGWRITER_H

#include "THzImage/common/iImageWriter.h"
#include "THzImage/common/pixel.h"

#include <string_view>

namespace Terrahertz::PNG {

/// @brief Writes an image to a file using the Portable-Network-Graphics format.
class Writer : public IImageWriter<BGRAPixel>
{
public:
    /// @brief Initializes a nwe PNGWriter.
    ///
    /// @param filepath The path to write the PNG-File to.
    Writer(std::string_view const filepath) noexcept;

    /// @copydoc IImageWriter::init
    bool init() noexcept override;

    /// @copydoc IImageWriter::write
    bool write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept override;

    /// @copydoc IImageWriter::deinit
    void deinit() noexcept override;

private:
    /// @brief The path to write the PNG-File to.
    std::string_view const _filepath;
};

} // namespace Terrahertz::PNG

#endif // !THZ_IMAGE_IO_PNGWRITER_H
