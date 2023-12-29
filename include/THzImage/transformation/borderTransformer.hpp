#ifndef THZ_IMAGE_HANDLING_BORDERTRANSFORMER_HPP
#define THZ_IMAGE_HANDLING_BORDERTRANSFORMER_HPP

#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/iImageTransformer.hpp"

#include <cstdint>
#include <limits>

namespace Terrahertz {

/// @brief Contains the borders to add to an image.
struct Borders
{
    /// @brief The amount of pixels to add to the top.
    std::uint8_t top{};

    /// @brief The amount of pixels to add to the right.
    std::uint8_t right{};

    /// @brief The amount of pixels to add to the bottom.
    std::uint8_t bottom{};

    /// @brief The amount of pixels to add to the left.
    std::uint8_t left{};
};

/// @brief Transformer adding a border to an image.
///
/// @tparam TPixelType The type of pixel used by the transformer.
template <typename TPixelType>
class BorderTransformer : public IImageTransformer<TPixelType>
{
public:
    /// @brief Initializes a new BorderTransformer using the given values.
    ///
    /// @param base The base transformer to wrap.
    /// @param borders The borders to add to the image.
    /// @param color The color of the borders.
    BorderTransformer(IImageTransformer<TPixelType> &base, Borders const borders, TPixelType const color) noexcept
        : _base{base}, _borders{borders}, _color{color}
    {
        setup();
    }

    /// @copydoc IImageTransformer::dimensions
    Rectangle dimensions() const noexcept override { return _dimensions; }

    /// @copydoc IImageTransformer::transform
    bool transform(TPixelType &pixel) noexcept override
    {
        if (_nextFlip > 0)
        {
            --_nextFlip;
            return _base.transform(pixel);
        }
        if (_nextFlip < 0)
        {
            ++_nextFlip;
            pixel = _color;
            return true;
        }
        // _nextFlip == 0
        flip();
        return transform(pixel);
    }

    /// @copydoc IImageTransformer::skip
    bool skip() noexcept override
    {
        if (_nextFlip > 0)
        {
            --_nextFlip;
            return _base.skip();
        }
        if (_nextFlip < 0)
        {
            ++_nextFlip;
            return true;
        }
        // _nextFlip == 0
        flip();
        return skip();
    }

    /// @copydoc IImageTransformer::reset
    bool reset() noexcept override
    {
        if (!_base.reset())
        {
            return false;
        }
        setup();
        return true;
    }

    /// @copydoc IImageTransformer::nextImage
    bool nextImage() noexcept override
    {
        if (!_base.nextImage())
        {
            return false;
        }
        setup();
        return true;
    }

private:
    /// @brief Sets up the transformer.
    void setup() noexcept
    {
        _baseDimensions    = _base.dimensions();
        _dimensions.width  = _baseDimensions.width + _borders.left + _borders.right;
        _dimensions.height = _baseDimensions.height + _borders.top + _borders.bottom;
        _stage             = 0U;
        _y                 = 0U;
        flip();
    }

    /// @brief Flips between border and image.
    void flip() noexcept
    {
        switch (_stage)
        {
        case 0U: // top
        {
            _nextFlip = (_dimensions.width * _borders.top) + _borders.left;
            _nextFlip *= -1;
            _stage = 1U;
            break;
        }
        case 1U: // image
        {
            _nextFlip = _baseDimensions.width;
            _stage    = 2U;
            break;
        }
        case 2U: // right/left/bottom
        {
            ++_y;
            if (_y < _baseDimensions.height)
            {
                _nextFlip = (_borders.left + _borders.right);
                _stage    = 1U;
            }
            else
            {
                _nextFlip = (_dimensions.width * _borders.bottom) + _borders.right;
                _stage    = 3U;
            }
            _nextFlip *= -1;
            break;
        }
        case 3U: // stop
        {
            _nextFlip = std::numeric_limits<std::int16_t>::max();
            break;
        }
        }
    }

    /// @brief The base transformer to wrap.
    IImageTransformer<TPixelType> &_base;

    /// @brief The borders to add to the image.
    Borders _borders;

    /// @brief The color of the borders.
    TPixelType _color;

    /// @brief The dimensions of the base image.
    Rectangle _baseDimensions{};

    /// @brief The dimensions of the resulting image.
    Rectangle _dimensions{};

    /// @brief The pixels to put out before the next flip between border and image.
    std::int16_t _nextFlip{};

    /// @brief The stage of the output.
    std::uint8_t _stage{};

    /// @brief The y-position in the output.
    std::uint32_t _y{};
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_HANDLING_BORDERTRANSFORMER_HPP
