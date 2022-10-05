#ifndef THZ_IMAGE_COMMON_IIMAGETRANSFORMER_H
#define THZ_IMAGE_COMMON_IIMAGETRANSFORMER_H

#include "THzCommon/math/rectangle.h"
#include "pixel.h"

namespace Terrahertz {

/// @brief Interface for all classes performing transformations on image data.
///
/// @tparam TPixelType The pixel type used by the transformer.
template <typename TPixelType>
class IImageTransformer
{
public:
    static_assert(is_pixel_type<TPixelType>::value, "TPixelType is not a known pixel type");

    /// @brief Shortcut to the used pixel type.
    using PixelType = TPixelType;

    /// @brief Default the destructor to make it virtual.
    virtual ~IImageTransformer() noexcept {}

    /// @brief Returns the dimensions of the image the transformation results in.
    ///
    /// @return The dimensions of the image the transformation results in.
    virtual Rectangle dimensions() const noexcept = 0;

    /// @brief Transform the next pixel from the underlying image.
    ///
    /// @param pixel Output: The pixel of the result image.
    /// @return True of there are still more pixels, false otherwise.
    virtual bool transform(TPixelType &pixel) noexcept = 0;

    /// @brief Skips to the next pixel.
    ///
    /// @return True if there are still more pixels, false otherwise.
    virtual bool skip() noexcept = 0;

    /// @brief Resets the transformer to restart the transformation, if the underlying data has changed.
    ///
    /// @return True if the transformer was reset, false otherwise.
    virtual bool reset() noexcept = 0;
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_COMMON_IIMAGETRANSFORMER_H
