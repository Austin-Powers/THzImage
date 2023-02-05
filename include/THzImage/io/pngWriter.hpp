#ifndef THZ_IMAGE_IO_PNGWRITER_HPP
#define THZ_IMAGE_IO_PNGWRITER_HPP

#include "THzImage/common/iImageWriter.hpp"
#include "THzImage/common/pixel.hpp"

#include <filesystem>

namespace Terrahertz::PNG {

/// @brief Writes an image to a file using the Portable-Network-Graphics format.
class Writer : public IImageWriter<BGRAPixel>
{
public:
    /// @brief Initializes a nwe PNGWriter.
    ///
    /// @param filepath The path to write the PNG-File to.
    Writer(std::filesystem::path const filepath) noexcept;

    /// @copydoc IImageWriter::init
    bool init() noexcept override;

    /// @copydoc IImageWriter::write
    bool write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept override;

    /// @copydoc IImageWriter::deinit
    void deinit() noexcept override;

private:
    /// @brief The path to write the PNG-File to.
    std::filesystem::path const _filepath;
};

} // namespace Terrahertz::PNG

#endif // !THZ_IMAGE_IO_PNGWRITER_HPP
