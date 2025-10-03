#ifndef THZ_IMAGE_HANDLING_BUFFERTRANSFORMER_HPP
#define THZ_IMAGE_HANDLING_BUFFERTRANSFORMER_HPP

#include "THzImage/common/iImageTransformer.hpp"
#include "THzImage/handling/imageRingBuffer.hpp"

#include <cstddef>

namespace Terrahertz {

/// @brief Transformer starting a chain of transformers from a imageRingBuffer.
///
/// @tparam TPixelType The type of pixel used by the buffer.
template <Pixel TPixelType>
class BufferTransformer : public IImageTransformer<TPixelType>
{
public:
    /// @brief Default initializes a BufferTransformer.
    BufferTransformer() noexcept = default;

    /// @brief Updates the setup used by the BufferTransformer.
    ///
    /// @param buffer The buffer to get the images from.
    /// @param slotIdx The index of the slot in the buffer.
    /// @param callNext True if nextImage() should call next() on the buffer, false otherwise.
    /// @return True if update was successful, false otherwise.
    bool update(ImageRingBuffer<TPixelType> &buffer, size_t const slotIdx, bool callNext) noexcept
    {
        if (slotIdx < buffer.slots())
        {
            _buffer   = &buffer;
            _slotIdx  = slotIdx;
            _callNext = callNext;
            updateView();
            return true;
        }
        return false;
    }

    /// @copydoc IImageTransformer::dimensions
    Rectangle dimensions() const noexcept override { return _view.dimensions(); }

    /// @copydoc IImageTransformer::transform
    bool transform(TPixelType &pixel) noexcept override { return _view.transform(pixel); }

    /// @copydoc IImageTransformer::skip
    bool skip() noexcept override { return _view.skip(); }

    /// @copydoc IImageTransformer::reset
    bool reset() noexcept override
    {
        if (_buffer != nullptr)
        {
            // this method could be called before update(...) was called, rendering _buffer == nullptr
            updateView();
        }
        return true;
    }

    /// @copydoc IImageTransformer::nextImage
    bool nextImage() noexcept override
    {
        if (_callNext)
        {
            if (_buffer->next())
            {
                updateView();
                return true;
            }
        }
        return false;
    }

private:
    /// @brief Updates the view of the transformer.
    void updateView() noexcept { _view = _buffer->operator[](_slotIdx).view(); }

    /// @brief The buffer to get the image from.
    ImageRingBuffer<TPixelType> *_buffer{};

    /// @brief The index of the slot in the buffer.
    size_t _slotIdx{};

    /// @brief True if nextImage() should call next() on the buffer, false otherwise.
    bool _callNext{};

    /// @brief The view of the current image.
    ImageView<TPixelType> _view{};
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_HANDLING_BUFFERTRANSFORMER_HPP
