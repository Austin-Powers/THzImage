#ifndef THZ_IMAGE_IO_GIFWRITER_HPP
#define THZ_IMAGE_IO_GIFWRITER_HPP

#include "THzCommon/structures/octree.hpp"
#include "THzImage/common/iImageWriter.hpp"
#include "THzImage/common/pixel.hpp"

#include <cstddef>
#include <gsl/gsl>
#include <string_view>

namespace Terrahertz::GIF {
namespace Internal {

/// @brief Encapsulates the algorithm for reducing the colors of a given image to 255.
class ColorReduction
{
public:
    /// @brief The target colors, GIF supports 256 but we reserve one for transparency.
    static constexpr size_t TargetColors{255U};

    /// @brief Analyzes the given buffer of pixels for selecting the colors.
    ///
    /// @param buffer The buffer of pixels to select the colors for.
    void analyze(gsl::span<BGRAPixel const> const buffer) noexcept;

    /// @brief Returns the color table.
    ///
    /// @return The color table.
    gsl::span<BGRAPixel const> colorTable() const noexcept;

    /// @brief Converts the given color into an index in the color table.
    ///
    /// @param color The color to convert.
    /// @return The index in the color table that matches the color as closly as possible.
    std::uint8_t convert(BGRAPixel const &color) const noexcept;

private:
    /// @brief The type of octree to reduction is based on.
    using BGROctreeType = Octree<std::uint8_t, std::uint16_t, std::uint8_t>;

    /// @brief The number of colors used in the color table.
    std::uint8_t _colorCount{2U};

    /// @brief The color table of the reduction.
    std::array<BGRAPixel, TargetColors> _colorTable{BGRAPixel{0xFFU, 0xFFU, 0xFFU}};

    /// @brief An octree for quickly finding the index related to a given color.
    BGROctreeType _quickAccess{};
};

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
