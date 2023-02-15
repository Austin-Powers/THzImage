#ifndef THZ_IMAGE_HANDLING_IMAGERINGBUFFER_HPP
#define THZ_IMAGE_HANDLING_IMAGERINGBUFFER_HPP

#include "THzImage/common/iImageReader.hpp"
#include "THzImage/common/iImageTransformer.hpp"
#include "THzImage/common/image.hpp"
#include "THzImage/common/pixel.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace Terrahertz {

/// @brief A Ringbuffer for image handling.
///
/// @tparam TPixelType The type of pixel used by the image.
/// @tparam TSlots The amount of images this buffer holds.
template <typename TPixelType, size_t TSlots>
class ImageRingBuffer
{
public:
    static_assert(TSlots > 0U, "ImageRingBuffer must have at least one slot");

    /// @brief IImageTransformer that connects to the ImageRingBuffer.
    class Transformer : public IImageTransformer<TPixelType>
    {};

    /// @brief Initializes a new ImageRingBuffer using the given reader for retrieving new images.
    ///
    /// @param reader The reader to get new images from.
    ImageRingBuffer(IImageReader<TPixelType> &reader) noexcept : _reader{&reader} { setupMap(); }

    /// @brief Initializes a new ImageRingBuffer using the given transofmer for retrieving new images.
    ///
    /// @param transformer The transformer to get new images from.
    ImageRingBuffer(IImageTransformer<TPixelType> &transformer) noexcept : _transformer{&transformer} { setupMap(); }

    /// @brief Returns the number of slots of this ring buffer.
    ///
    /// @return The number of slots of this ring buffer.
    constexpr size_t slots() const noexcept { return TSlots; }

    /// @brief Provides access to the images in the buffer.
    ///
    /// @param index The index of the image, newest images has index 0, second newest 1 and so on.
    /// @return The image at the given index.
    Image<TPixelType> &operator[](size_t const index) noexcept { return *_map[index]; }

    /// @brief Provides access to the images in the buffer.
    ///
    /// @param index The index of the image, newest images has index 0, second newest 1 and so on.
    /// @return The image at the given index.
    Image<TPixelType> const &operator[](size_t const index) const noexcept { return *_map[index]; }

    /// @brief Loads the next image either from the reader or the transformer.
    ///
    /// @return True if the next image was loaded, false otherwise.
    bool next() noexcept
    {
        if (_reader != nullptr)
        {
            if ((!_reader->imagePresent()) || (!_map[TSlots - 1U]->read(_reader)))
            {
                return false;
            }
        }
        else
        {
            if ((_count > 0U) && !_transformer->nextImage())
            {
                return false;
            }
            if (!_map[TSlots - 1U]->storeResultOf(_transformer))
            {
                return false;
            }
        }
        std::rotate(_map.rbegin(), _map.rbegin() + 1U, _map.rend());
        ++_count;
        return true;
    }

    /// @brief Returns the total amount of images loaded by this buffer.
    ///
    /// @return The total amount of images loaded by this buffer.
    size_t count() const noexcept { return _count; }

private:
    /// @brief Sets up the map for the buffer.
    void setupMap() noexcept
    {
        for (auto i = 0U; i < TSlots; ++i)
        {
            _map[i] = &_buffer[i];
        }
    }

    /// @brief The buffer of images.
    std::array<Image<TPixelType>, TSlots> _buffer{};

    /// @brief The mapping of the buffer newest to oldest entry.
    std::array<Image<TPixelType> *, TSlots> _map{};

    /// @brief Pointer to the reader used to get new images.
    IImageReader<TPixelType> *_reader{};

    /// @brief Pointer to the transformer used to get new images.
    IImageTransformer<TPixelType> *_transformer{};

    /// @brief Counter for the loaded images.
    size_t _count{};
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_HANDLING_IMAGERINGBUFFER_HPP
