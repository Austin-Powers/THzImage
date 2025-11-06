#ifndef THZ_IMAGE_HANDLING_ASYNCIMAGERINGBUFFER_HPP
#define THZ_IMAGE_HANDLING_ASYNCIMAGERINGBUFFER_HPP

#include "THzCommon/utility/workerThread.hpp"
#include "THzImage/handling/imageRingBuffer.hpp"

#include <iostream>

namespace Terrahertz {

/// @brief Extends the basic ImageRingBuffer by adding the ability to retrieve the reader or transformer result
/// asynchronously.
///
/// @tparam TPixelType The type of pixel used by the image.
template <Pixel TPixelType>
class AsyncImageRingBuffer : public ImageRingBuffer<TPixelType>
{
public:
    /// @brief Initializes a new ImageRingBuffer using the given reader for retrieving new images.
    ///
    /// @param reader The reader to get new images from.
    /// @param slots The amount of images this buffer holds.
    AsyncImageRingBuffer(IImageReader<TPixelType> &reader, size_t const slots) noexcept
        : ImageRingBuffer<TPixelType>{reader, slots + 1U}
    {
        _worker.thread = std::thread([this]() { threadMethod(); });
    }

    /// @brief Initializes a new ImageRingBuffer using the given transofmer for retrieving new images.
    ///
    /// @param transformer The transformer to get new images from.
    /// @param slots The amount of images this buffer holds.
    AsyncImageRingBuffer(IImageTransformer<TPixelType> &transformer, size_t const slots) noexcept
        : ImageRingBuffer<TPixelType>{transformer, slots + 1U}
    {
        _worker.thread = std::thread([this]() { threadMethod(); });
    }

    /// @brief Finalizes this AsyncImageRingBuffer instance.
    ~AsyncImageRingBuffer() noexcept { _worker.shutdown(); }

    /// @copydoc ImageRingBuffer::slots
    [[nodiscard]] size_t slots() const noexcept override { return ImageRingBuffer<TPixelType>::slots() - 1U; }

    /// @copydoc ImageRingBuffer::next
    bool next() noexcept
    {
        WorkerThread::UniqueLock lock{_worker.mutex};
        while (_reading)
        {
            _worker.wakeUp.wait_for(lock, std::chrono::milliseconds{50});
        }
        auto const result = _readingResult;
        if (result)
        {
            ImageRingBuffer<TPixelType>::skip();
        }
        _reading = true;
        _worker.wakeUp.notify_one();
        return result;
    }

private:
    /// @brief The method for the worker thread.
    void threadMethod() noexcept
    {
        WorkerThread::UniqueLock lock{_worker.mutex};
        while (!_worker.shutdownFlag)
        {
            if (!_worker.wakeUp.wait_for(lock, std::chrono::milliseconds{50}, [&]() { return _reading; }))
            {
                continue;
            }
            _readingResult = ImageRingBuffer<TPixelType>::loadNextImage();
            _reading       = false;
            _worker.wakeUp.notify_one();
        }
    }

    /// @brief The worker thread of the buffer.
    WorkerThread _worker{};

    /// @brief Flag to signal the thread to perform the next read operation.
    bool _reading{true};

    /// @brief The result of the last reading operation.
    bool _readingResult{};
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_HANDLING_ASYNCIMAGERINGBUFFER_HPP
