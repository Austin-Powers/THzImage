#ifndef THZ_IMAGE_IO_GIFWRITER_HPP
#define THZ_IMAGE_IO_GIFWRITER_HPP

#include "THzImage/common/iImageWriter.hpp"
#include "THzImage/common/pixel.hpp"

#include <string_view>

namespace Terrahertz::GIF {
namespace Internal {

/// @brief Encapsulates the algorithm for reducing the colors of a given image to 255.
class ColorReduction
{};

} // namespace Internal

/// @brief Writes an image to a file using the GIF format.
class Writer : public IImageWriter<BGRAPixel>
{
public:
    /// @brief Initializes a new GIF::Writer.
    ///
    /// @param filepath The path to write the GIF-File to.
    Writer(std::string_view const filepath) noexcept;

    /// @copydoc IImageWriter::init
    bool init() noexcept override;

    /// @copydoc IImageWriter::write
    bool write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept override;

    /// @copydoc IImageWriter::deinit
    void deinit() noexcept override;
};

} // namespace Terrahertz::GIF

#endif // !THZ_IMAGE_IO_BMPWRITER_HPP
