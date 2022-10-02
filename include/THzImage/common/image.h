#ifndef THZ_IMAGE_COMMON_IMAGE_H
#define THZ_IMAGE_COMMON_IMAGE_H

#include "THzCommon/math/rectangle.h"
#include "pixel.h"

namespace Terrahertz {

/// @brief Class representing raster based images.
///
/// @tparam TPixelType The type of pixel used by the image.
template <typename TPixelType>
class Image
{
public:
    static_assert(is_pixel_type<TPixelType>::value, "TPixelType is not a known pixel type");

    /// @brief Default initializes a new image.
    Image() noexcept = default;

    /// @brief Returns the dimensions of the image.
    ///
    /// @return The dimensions of the image.
    Rectangle const &dimensions() const noexcept { return _dimensions; }

    /// @brief Sets the new dimensions of the image.
    ///
    /// @param dim The new dimensions of the image.
    /// @return True if the image was resized correctly, false otherwise.
    bool setDimensions(Rectangle const &dim) noexcept
    {
        _dimensions = dim;
        return true;
    }

private:
    /// @brief The dimensions of the image.
    Rectangle _dimensions{};
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_COMMON_IMAGE_H
