#ifndef THZ_IMAGE_HANDLING_IMAGERINGBUFFER_HPP
#define THZ_IMAGE_HANDLING_IMAGERINGBUFFER_HPP

#include "THzImage/common/iImageReader.hpp"
#include "THzImage/common/iImageTransformer.hpp"
#include "THzImage/common/image.hpp"
#include "THzImage/common/pixel.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace Terrahertz {

/// @brief A Ringbuffer for image handling.
///
/// @tparam TPixelType The type of pixel used by the image.
template <Pixel TPixelType>
class ImageRingBuffer
{
public:
    /// @brief Initializes a new ImageRingBuffer using the given reader for retrieving new images.
    ///
    /// @param reader The reader to get new images from.
    /// @param slots The amount of images this buffer holds.
    ImageRingBuffer(IImageReader<TPixelType> &reader, size_t const slots) noexcept : _reader{&reader}, _slots{slots}
    {
        setup();
    }

    /// @brief Initializes a new ImageRingBuffer using the given transofmer for retrieving new images.
    ///
    /// @param transformer The transformer to get new images from.
    /// @param slots The amount of images this buffer holds.
    /// @param forwardNext True if nextImage of the transformer shall be called on next(), false otherwise.
    ImageRingBuffer(IImageTransformer<TPixelType> &transformer,
                    size_t const                   slots,
                    bool const                     forwardNext = true) noexcept
        : _transformer{&transformer}, _slots{slots}, _forwardNext{forwardNext}
    {
        setup();
    }

    /// @brief Default destructor to make it virtual.
    virtual ~ImageRingBuffer() noexcept = default;

    /// @brief Returns the number of slots of this ring buffer.
    ///
    /// @return The number of slots of this ring buffer.
    [[nodiscard]] virtual size_t slots() const noexcept { return _slots; }

    /// @brief Provides access to the images in the buffer.
    ///
    /// @param index The index of the image, newest images has index 0, second newest 1 and so on.
    /// @return The image at the given index.
    [[nodiscard]] Image<TPixelType> &operator[](size_t const index) noexcept { return *_map[index]; }

    /// @brief Provides access to the images in the buffer.
    ///
    /// @param index The index of the image, newest images has index 0, second newest 1 and so on.
    /// @return The image at the given index.
    [[nodiscard]] Image<TPixelType> const &operator[](size_t const index) const noexcept { return *_map[index]; }

    /// @brief Loads the next image either from the reader or the transformer.
    ///
    /// @param countFailure If true failures increase the counter as well.
    /// @return True if the next image was loaded, false otherwise.
    virtual bool next(bool const countFailure = false) noexcept
    {
        if (loadNextImage())
        {
            skip();
            return true;
        }
        if (countFailure)
        {
            skip(true);
        }
        return false;
    }

    /// @brief Performs the update of map and increments the counter without loading the next image.
    ///
    /// @param keepMapping True if updating the map shall be omitted.
    void skip(bool keepMapping = false) noexcept
    {
        if (!keepMapping)
        {
            std::rotate(_map.rbegin(), _map.rbegin() + 1U, _map.rend());
        }
        ++_count;
    }

    /// @brief Returns the total amount of images loaded by this buffer.
    ///
    /// @return The total amount of images loaded by this buffer.
    [[nodiscard]] size_t count() const noexcept { return _count; }

protected:
    /// @brief Loads the next image of the reader/transformer into the last slot.
    ///
    /// @return True if the operation was successful, false otherwise.
    bool loadNextImage() noexcept
    {
        if (_reader != nullptr)
        {
            if ((!_reader->imagePresent()) || (!_map[_slots - 1U]->readFrom(*_reader)))
            {
                return false;
            }
        }
        else
        {
            if (_forwardNext && (_count > 0U))
            {
                // start calling nextImage after the first image has been processed
                if (!_transformer->nextImage())
                {
                    return false;
                }
            }
            if (!_map[_slots - 1U]->executeAndIngest(*_transformer))
            {
                return false;
            }
        }
        return true;
    }

private:
    /// @brief Sets up the vectors for the buffer.
    void setup() noexcept
    {
        _buffer.resize(_slots);
        _map.resize(_slots);
        for (auto i = 0U; i < _slots; ++i)
        {
            _map[i] = &_buffer[i];
        }
    }

    /// @brief The mapping of the buffer newest to oldest entry.
    std::vector<Image<TPixelType> *> _map{};

    /// @brief The buffer of images.
    std::vector<Image<TPixelType>> _buffer{};

    /// @brief Pointer to the reader used to get new images.
    IImageReader<TPixelType> *_reader{};

    /// @brief Pointer to the transformer used to get new images.
    IImageTransformer<TPixelType> *_transformer{};

    /// @brief The amount of images this buffer holds.
    size_t _slots{};

    /// @brief Counter for the loaded images.
    size_t _count{};

    /// @brief True if nextImage of the transformer shall be called on next(), false otherwise.
    bool _forwardNext{};
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_HANDLING_IMAGERINGBUFFER_HPP
