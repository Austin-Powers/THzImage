#ifndef THZ_IMAGE_IO_ASYNCWRITER_HPP
#define THZ_IMAGE_IO_ASYNCWRITER_HPP

#include "THzCommon/logging/logging.hpp"
#include "THzImage/common/iImageWriter.hpp"
#include "THzImage/common/image.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace Terrahertz {

/// @brief Uses a given writer to write images asynchronously.
///
/// @tparam TPixelType The type of pixel used by the images to write.
template <typename TPixelType>
class AsyncWriter
{
    /// @brief Name provider for the THzImage.IO.PNG.Writer class.
    struct Project
    {
        static constexpr char const *name() noexcept { return "THzImage.IO.AsyncWriter"; }
    };

public:
    /// @brief Initializes a new AsyncWriter using the given writer.
    ///
    /// @param writer The writer to wrap.
    AsyncWriter(IImageWriter<TPixelType> &writer) noexcept : _writer{writer}
    {
        _thread = std::thread([this]() { worker(); });
    }

    /// @brief Finalizes the AsyncWriter shutting down the internal thread.
    ~AsyncWriter() noexcept
    {
        _shutdown = true;
        _newImage.notify_one();
        _thread.join();
    }

    /// @brief Writes the given image using the wrapped writer.
    ///
    /// @param image The image to write.
    /// @return True if the image was accepted by the writer, false if the writer is busy writing the last image.
    bool write(Image<TPixelType> const &image) noexcept
    {
        if (_image != nullptr)
        {
            return false;
        }
        std::unique_lock lock{_mutex};
        if (_image != nullptr)
        {
            return false;
        }
        _image = &image;
        lock.unlock();
        _newImage.notify_one();
        return true;
    }

private:
    /// @brief The method for the worker-thread.
    void worker() noexcept
    {
        std::unique_lock lock{_mutex};
        while (!_shutdown)
        {
            _newImage.wait_for(
                lock, std::chrono::milliseconds{10U}, [this]() { return (_image != nullptr) || _shutdown; });
            if (_image != nullptr)
            {
                if (!_image->writeTo(&_writer))
                {
                    logMessage<LogLevel::Error, Project>("Unable to write image");
                }
                _image = nullptr;
            }
        }
    }

    /// @brief Reference to the writer to use.
    IImageWriter<TPixelType> &_writer{};

    /// @brief The current image to write.
    Image<TPixelType> const *_image{};

    /// @brief The worker thread.
    std::thread _thread{};

    /// @brief Flag signalling
    std::atomic_bool _shutdown{false};

    std::mutex _mutex{};

    std::condition_variable _newImage{};
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_IO_ASYNCWRITER_HPP
