#ifndef THZ_IMAGE_COMMON_IMAGE_H
#define THZ_IMAGE_COMMON_IMAGE_H

#include "THzCommon/math/rectangle.h"
#include "imageView.h"
#include "pixel.h"

#include <cstddef>
#include <vector>

namespace Terrahertz {

/// @brief Class representing raster based images.
///
/// @tparam TPixelType The type of pixel used by the image.
template <typename TPixelType>
class Image
{
public:
    static_assert(is_pixel_type<TPixelType>::value, "TPixelType is not a known pixel type");

    /// @brief Shortcut to the pixel type used by this image.
    using value_type = TPixelType;

    /// @brief Reference to a pixel of  this image.
    using reference = TPixelType &;

    /// @brief Reference to a pixel of  this image.
    using const_reference = TPixelType const &;

    /// @brief The size type of this image.
    using size_type = size_t;

    /// @brief Default initializes a new image.
    Image() noexcept = default;

    /// @brief Returns the dimensions of the image.
    ///
    /// @return The dimensions of the image.
    [[nodiscard]] Rectangle const &dimensions() const noexcept { return _dimensions; }

    /// @brief Sets the new dimensions of the image and resizes the internal buffer.
    ///
    /// @param dim The new dimensions of the image.
    /// @return True if the image was resized correctly, false otherwise.
    /// @remarks This operation scrambles the currently held image data.
    [[nodiscard]] bool setDimensions(Rectangle const &dim) noexcept
    {
        auto const newArea = dim.area();
        _data.resize(newArea);
        if (_data.size() != newArea)
        {
            return false;
        }
        _dimensions = dim;
        return true;
    }

    /// @brief Index operator of the image for retrieving pixels by index.
    ///
    /// @param index The index of the pixel to return.
    /// @return The pixel at the given index.
    [[nodiscard]] reference operator[](size_type index) noexcept { return _data[index]; }

    /// @brief Index operator of the image for retrieving pixels by index.
    ///
    /// @param index The index of the pixel to return.
    /// @return The pixel at the given index.
    [[nodiscard]] const_reference operator[](size_type index) const noexcept { return _data[index]; }

    /// @brief Returns a view of the entire image.
    ///
    /// @return A view of the entire image.
    [[nodiscard]] ImageView<TPixelType> view() noexcept { return ImageView<TPixelType>{_data.data(), _dimensions}; }

    /// @brief Returns a view of the entire image.
    ///
    /// @return A view of the entire image.
    [[nodiscard]] ImageView<TPixelType const> view() const noexcept
    {
        return ImageView<TPixelType const>{_data.data(), _dimensions};
    }

    /// @brief Returns a view of the given region of the image.
    ///
    /// @param region The region of the image to create the view of.
    /// @return A view of the region of the image.
    [[nodiscard]] ImageView<TPixelType> view(Rectangle const &region) noexcept
    {
        return ImageView<TPixelType>{_data.data(), _dimensions, region};
    }

    /// @brief Returns a view of the given region of the image.
    ///
    /// @param region The region of the image to create the view of.
    /// @return A view of the region of the image.
    [[nodiscard]] ImageView<TPixelType const> view(Rectangle const &region) const noexcept
    {
        return ImageView<TPixelType const>{_data.data(), _dimensions, region};
    }

private:
    /// @brief The dimensions of the image.
    Rectangle _dimensions{};

    /// @brief The memory holding the image data.
    std::vector<TPixelType> _data{};
};

/// @brief Using declaration for an image using BGRAPixel.
using BGRAImage = Image<BGRAPixel>;

/// @brief Using declaration for an image using HSVAPixel.
using BGRAImage = Image<BGRAPixel>;

} // namespace Terrahertz

#endif // !THZ_IMAGE_COMMON_IMAGE_H
