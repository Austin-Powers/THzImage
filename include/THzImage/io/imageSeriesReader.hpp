#ifndef THZ_IMAGE_IO_IMAGESERIESREADER_HPP
#define THZ_IMAGE_IO_IMAGESERIESREADER_HPP

#include "THzCommon/logging/logging.hpp"
#include "THzImage/common/iImageReader.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <string_view>

namespace Terrahertz::ImageSeries {

/// @brief Name provider for the THzImage.IO.ImageSeries.Reader class.
struct ReaderProject
{
    static constexpr char const *name() noexcept { return "THzImage.IO.ImageSeries.Reader"; }
};

/// @brief Wrapper for other readers, enabling reading entire directories of images.
///
/// @tparam TWrapped The type of the wrapped image reader.
template <FileImageReader TWrapped>
class Reader : public IImageReader<typename TWrapped::PixelType>
{
public:
    /// @brief Initializes a new ImageSeries::Reader.
    ///
    /// @param directoryPath The path of the directory from which to load the images.
    Reader(std::string_view const directoryPath) noexcept
    {
        if (!std::filesystem::exists(directoryPath))
        {
            logMessage<LogLevel::Error, ReaderProject>("directoryPath does not exist");
        }
        else if (!std::filesystem::is_directory(directoryPath))
        {
            logMessage<LogLevel::Error, ReaderProject>("directoryPath leads to a file");
        }
        else
        {
            _iter = std::filesystem::directory_iterator{directoryPath};
            cycleReader();
        }
    }

    /// @brief Returns the path of the current file to read.
    ///
    /// @return The path of the current file to read.
    std::filesystem::path currentFilepath() const noexcept
    {
        if (imagePresent())
        {
            return _iter->path();
        }
        return {};
    }

    /// @brief Skips the current file.
    void skipFile() noexcept
    {
        ++_iter;
        cycleReader();
    }

    /// @copydoc IImageReader::imagePresent
    bool imagePresent() const noexcept override { return _iter != std::filesystem::directory_iterator{}; }

    /// @copydoc IImageReader::init
    bool init() noexcept override
    {
        if (_wrapped != nullptr)
        {
            return _wrapped->init();
        }
        return false;
    }

    /// @copydoc IImageReader::dimensions
    Rectangle dimensions() const noexcept override
    {
        if (_wrapped != nullptr)
        {
            return _wrapped->dimensions();
        }
        return {};
    }

    /// @copydoc IImageReader::read
    bool read(gsl::span<typename TWrapped::PixelType> buffer) noexcept override
    {
        if (_wrapped != nullptr)
        {
            return _wrapped->read(buffer);
        }
        return false;
    }

    /// @copydoc IImageReader::deinit
    void deinit() noexcept override
    {
        closeReader();
        skipFile();
    }

private:
    /// @brief Closes the current reader, if one is open.
    void closeReader() noexcept
    {
        if (_wrapped != nullptr)
        {
            _wrapped->deinit();
            _wrapped->~TWrapped();
            _wrapped = nullptr;
        }
    }

    /// @brief Creates a reader for the current file of _iter.
    ///
    /// @remarks If fileTypeFits() returns false, this method will increment _iter and try again
    /// till a fitting file is found or the iterator is exhausted.
    void cycleReader() noexcept
    {
        // the iterator might give us a file that is not compatible with the reader type
        // so we have to check if it can be opened and skip the file if not
        for (; imagePresent(); ++_iter)
        {
            closeReader();
            _wrapped = new (_buffer.data()) TWrapped(*_iter);
            if (_wrapped == nullptr)
            {
                logMessage<LogLevel::Error, ReaderProject>("Creating the reader failed");
                break;
            }
            if (_wrapped->fileTypeFits())
            {
                break;
            }
        }
    }

    /// @brief The iterator for the directory where the images are stored.
    std::filesystem::directory_iterator _iter{};

    /// @brief Pointer to the wrapped reader.
    TWrapped *_wrapped{};

    /// @brief The buffer for the wrapped reader.
    std::array<std::uint8_t, sizeof(TWrapped)> _buffer{};
};

} // namespace Terrahertz::ImageSeries

#endif // !THZ_IMAGE_IO_IMAGESERIESREADER_HPP
