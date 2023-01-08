#ifndef THZ_IMAGE_IO_GIFWRITER_HPP
#define THZ_IMAGE_IO_GIFWRITER_HPP

#include "THzImage/common/iImageWriter.hpp"
#include "THzImage/common/pixel.hpp"

#include <array>
#include <cstddef>
#include <gsl/gsl>
#include <string_view>
#include <vector>

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
    /// @brief The subdivisions of the quick access cube.
    static constexpr size_t QALength{8U};

    /// @brief The entries in each cell of the quick access cube.
    static constexpr size_t QAEntries{16U};

    /// @brief The length of a quick access slice of the colorspace.
    static constexpr size_t QASlice{256U / QALength};

    /// @brief The number of colors used in the color table.
    std::uint8_t _colorCount{2U};

    /// @brief The color table of the reduction.
    std::array<BGRAPixel, TargetColors> _colorTable{BGRAPixel{0xFFU, 0xFFU, 0xFFU}};

    /// @brief The quick access cube for cutting down lookup time.
    ///
    /// @remarks Each cell of the cube contains a list of the colors closest to this cell.
    std::array<std::array<std::array<std::array<std::uint8_t, QAEntries>, QALength>, QALength>, QALength>
        _quickAccess{};
};

/// @brief Encapsulates the dithering algorithm.
class Dithering
{
public:
    /// @brief Sets the parameters for the dithering.
    ///
    /// @param width The width of the image.
    /// @param colorReduction The color reduction instance to use.
    void setParameters(std::uint32_t const width, ColorReduction const &colorReduction) noexcept;

    /// @brief Converts the given color into an index in the color table of the given colorReduction.
    ///
    /// @param color The color to convert.
    /// @return The index of the color in the table of the colorReduction.
    std::uint8_t convert(BGRAPixel color) noexcept;

private:
    /// @brief The width of the image.
    std::uint32_t _width{};

    /// @brief Counter for the row of the image.
    std::uint32_t _rowCounter{};

    /// @brief The color reduction instance to use.
    ColorReduction const *_colorReduction{};

    /// @brief The floating point parts of the dithering process.
    std::vector<BGRAPixelFloat> _floatingParts{};

    /// @brief Pointers to the pixels to currently work with.
    std::array<BGRAPixelFloat *, 6U> _pixel{};
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
