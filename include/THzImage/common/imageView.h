#ifndef THZ_IMAGE_COMMON_IMAGEVIEW_H
#define THZ_IMAGE_COMMON_IMAGEVIEW_H

#include "THzCommon/math/point.h"
#include "THzCommon/math/rectangle.h"

#include <cstddef>
#include <type_traits>

namespace Terrahertz {

/// @brief A view into a certain region of an image or pixel buffer.
///
/// @tparam TValueType The type of pixel value for this view.
template <typename TValueType>
class ImageView
{
public:
    /// @brief The value type of the view.
    using value_type = TValueType;

    /// @brief Shortcut to the pixel type used by this view.
    using pixel_type = std::remove_cv_t<TValueType>;

    /// @brief The difference type of this iterator.
    using difference_type = ptrdiff_t;

    /// @brief The size type of this iterator.
    using size_type = size_t;

    /// @brief Pointer to a pixel of this view.
    using pointer = value_type *;

    /// @brief Reference to a pixel of this view.
    using reference = value_type &;

    /// @brief Default initializes a new ImageView.
    ImageView() noexcept = default;

    /// @brief Initializes a new ImageView using the given values.
    ///
    /// @param basePtr The pointer to the beginning of the image buffer.
    /// @param imageDim The dimensions of the image buffer.
    ImageView(pointer const basePtr, Rectangle const &imageDim) noexcept
        : _basePointer{basePtr}, _imageDimensions{imageDim}, _region{imageDim}
    {
        _imageDimensions.upperLeftPoint = {};
        _region.upperLeftPoint          = {};
        reset();
    }

    /// @brief Initializes a new ImageView using the given values.
    ///
    /// @param basePtr The pointer to the beginning of the image buffer.
    /// @param imageDim The dimensions of the image buffer.
    /// @param region The region inside the image buffer.
    ImageView(pointer const basePtr, Rectangle const &imageDim, Rectangle const &region) noexcept
        : _basePointer{basePtr}, _imageDimensions{imageDim}, _region{region}
    {
        _imageDimensions.upperLeftPoint = {};

        _region = _imageDimensions.intersection(region);
        reset();
    }

    /// @brief Reset the location of this view to theupper left corner of its region.
    void reset() noexcept
    {
        _currentPosition = _region.upperLeftPoint;
        _currentPointer =
            _basePointer + (static_cast<ptrdiff_t>(_currentPosition.y) * _imageDimensions.width) + _currentPosition.x;
    }

    /// @brief Creates a sub view of this image view by intersecting the region with the given subRegion.
    ///
    /// @param subRegion The region to intersect with the region of this view.
    /// @return The sub view.
    [[nodiscard]] ImageView subView(Rectangle const subRegion) const noexcept
    {
        return ImageView{_basePointer, _imageDimensions, _region.intersection(subRegion)};
    }

    // operators
    /// @brief Returns a pointer to the current location in the image buffer.
    ///
    /// @return A pointer to the current location in the image buffer.
    [[nodiscard]] pointer operator->() const noexcept { return _currentPointer; }

    /// @brief Returns a reference to the current location in the image buffer.
    ///
    /// @return A reference to the current location in the image buffer.
    [[nodiscard]] reference operator*() const noexcept { return *_currentPointer; }

    /// @brief Increments the position of the image view inside its region in the image buffer.
    ///
    /// @return The reference to the image view.
    ImageView &operator++() noexcept
    {
        ++_currentPosition.x;
        ++_currentPointer;
        if (_currentPosition.x == _region.upperLeftPoint.x + _region.width)
        {
            _currentPointer += (_imageDimensions.width - _region.width);
            _currentPosition.x = _region.upperLeftPoint.x;
            ++_currentPosition.y;
        }
        return *this;
    }

    /// @brief Increments the position of the ImageView inside its zone in the image buffer.
    ///
    /// @return A copy of the image view before incrementing.
    ImageView operator++(int) noexcept
    {
        ImageView temp = *this;
        ++*this;
        return temp;
    }

    // getters
    /// @brief Returns the pointer to the start of the image buffer.
    ///
    /// @return The pointer to the base image.
    [[nodiscard]] pointer basePointer() const noexcept { return _basePointer; }

    /// @brief Returns the dimensions of the base image.
    ///
    /// @return The dimensions of the base image.
    [[nodiscard]] Rectangle const &imageDimensions() const noexcept { return _imageDimensions; }

    /// @brief Returns the region of the view.
    ///
    /// @return The region of the view.
    [[nodiscard]] Rectangle const &region() const noexcept { return _region; }

    /// @brief Returns the current position of the view.
    ///
    /// @return The current position of the view.
    [[nodiscard]] Point const &currentPosition() const noexcept { return _currentPosition; }

private:
    /// @brief The pointer to the start of the image buffer.
    pointer _basePointer{};

    /// @brief The dimensions of the image buffer.
    Rectangle _imageDimensions{};

    /// @brief The region of the view.
    Rectangle _region{};

    /// @brief The pointer to the current location in the image buffer.
    pointer _currentPointer{};

    /// @brief The current position of the view.
    Point _currentPosition{};
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_COMMON_IMAGEVIEW_H
