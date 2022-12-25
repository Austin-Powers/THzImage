#ifndef THZ_IMAGE_TRANSFORMATION_COLORREDUCTION_HPP
#define THZ_IMAGE_TRANSFORMATION_COLORREDUCTION_HPP

#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/iImageTransformer.hpp"
#include "THzImage/common/pixel.hpp"

#include <cstdint>
#include <gsl/gsl>
#include <optional>

namespace Terrahertz {

/// @brief Class encapsulating different color reduction related algorithms.
class ColorReduction : public IImageTransformer<BGRAPixel>
{
public:
    /// @brief Initializes a new ColorReduction transformer using another transformeras base.
    ///
    /// @param base The base transformer to use.
    /// @param gray Flag signalling if the image shall be reduced to gray-scale (true) or colors (false).
    /// @param targetCount The target color count.
    /// @param dithering Flag signalling if the output shall be dithered.
    ColorReduction(IImageTransformer<BGRAPixel> &base,
                   bool const                    gray,
                   std::uint16_t const           targetCount = 256U,
                   bool const                    dithering   = true) noexcept;

    /// @brief Initializes a new ColorReduction transformer using a buffer to transform.
    ///
    /// @param buffer The buffer containing the image data.
    /// @param dimensions The dimensions of the buffer.
    /// @param gray Flag signalling if the image shall be reduced to gray-scale (true) or colors (false).
    /// @param targetCount The target color count.
    /// @param dithering Flag signalling if the output shall be dithered.
    ColorReduction(gsl::span<BGRAPixel const> buffer,
                   Rectangle const           &dimensions,
                   bool const                 gray,
                   std::uint16_t const        targetCount = 256U,
                   bool const                 dithering   = true) noexcept;

    /// @copydoc IImageTransformer::dimensions
    Rectangle dimensions() const noexcept override;

    /// @copydoc IImageTransformer::transform
    bool transform(BGRAPixel &pixel) noexcept override;

    /// @copydoc IImageTransformer::skip
    bool skip() noexcept override;

    /// @copydoc IImageTransformer::reset
    bool reset() noexcept override;

    /// @brief Returns the color table used for the color reduction.
    ///
    /// @return The color table used for color reduction.
    gsl::span<BGRAPixel const> colorTable() noexcept;

    /// @brief Transforms the next pixel fro the underlying image into an index of the color table.
    ///
    /// @return The index of the next pixel in the color table, if there are pixels left.
    std::optional<std::uint16_t> transformToIndex() noexcept;
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_TRANSFORMATION_COLORREDUCTION_HPP
