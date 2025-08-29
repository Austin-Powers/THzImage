#ifndef THZ_IMAGE_IO_IMAGESERIESWRITER_HPP
#define THZ_IMAGE_IO_IMAGESERIESWRITER_HPP

#include "THzCommon/logging/logging.hpp"
#include "THzImage/common/iImageWriter.hpp"

#include <array>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <optional>
#include <string_view>

namespace Terrahertz::ImageSeries {

/// @brief Name provider for the THzImage.IO.ImageSeries.Writer class.
struct WriterProject
{
    static constexpr char const *name() noexcept { return "THzImage.IO.ImageSeries.Writer"; }
};

/// @brief Wrapper for other writers, enabling writing of multiple images.
///
/// @tparam TWrapped The type of the wrapped image writer.
template <FileImageWriter TWrapped>
class Writer : public IImageWriter<typename TWrapped::PixelType>
{
public:
    using IImageWriter<typename TWrapped::PixelType>::writeContentOf;

    /// @brief Creates a writer using the given filepath as a template.
    ///
    /// @param filepath The path for the files, has to contain a single '?' to signify where the numbering shall go.
    /// @param startNumber The number of the first file to store.
    /// @param increments The increments in which to increase the number of the images.
    /// @return The new writer, if filepath contained a single '?' for the numbering.
    [[nodiscard]] static std::optional<Writer> createWriter(std::string_view const filepath,
                                                            std::uint32_t const    startNumber = 0U,
                                                            std::uint32_t const    increments  = 1U) noexcept
    {
        if (increments == 0U)
        {
            return {};
        }
        // ? => %06d
        // -1 + 4 = 3
        if ((filepath.size() + 3U) > PathBufferSize)
        {
            return {};
        }
        if (std::count(filepath.cbegin(), filepath.cend(), '?') != 1)
        {
            return {};
        }
        return Writer(filepath, startNumber, increments);
    }

    /// @copydoc IImageWriter::init
    bool init() noexcept override
    {
        closeWriter();
        std::array<char, PathBufferSize + 4U> filepath{};
        std::snprintf(filepath.data(), filepath.size(), _filepath.data(), _nextNumber);
        _wrapped = new (_buffer.data()) TWrapped(filepath.data());
        if (_wrapped == nullptr)
        {
            logMessage<LogLevel::Error, WriterProject>("Creating the writer failed");
            return false;
        }
        return _wrapped->init();
    }

    /// @copydoc IImageWriter::write
    bool write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept override
    {
        if (_wrapped != nullptr)
        {
            return _wrapped->write(dimensions, buffer);
        }
        return false;
    }

    /// @copydoc IImageWriter::deinit
    void deinit() noexcept override
    {
        if (_wrapped != nullptr)
        {
            _wrapped->deinit();
        }
        closeWriter();
        _nextNumber += _increments;
    }

private:
    /// @brief The size of the path buffer.
    static constexpr size_t PathBufferSize{512U};

    /// @brief Initializes a new ImageSeries::Writer using the given filepath.
    ///
    /// @param filepath The filepath template for the file to write.
    /// @param startNumber The number of the first file to store.
    /// @param increments The increments in which to increase the number of the images.
    Writer(std::string_view const filepath, std::uint32_t const startNumber, std::uint32_t const increments) noexcept
        : _nextNumber{startNumber}, _increments{increments}
    {
        auto bufferPos = 0U;
        for (auto const c : filepath)
        {
            if (c != '?')
            {
                _filepath[bufferPos] = c;
            }
            else
            {
                _filepath[bufferPos]   = '%';
                _filepath[++bufferPos] = '0';
                _filepath[++bufferPos] = '6';
                _filepath[++bufferPos] = 'd';
            }
            ++bufferPos;
        }
    }

    /// @brief Closes the current writer, if one is open.
    void closeWriter() noexcept
    {
        if (_wrapped != nullptr)
        {
            _wrapped->~TWrapped();
            _wrapped = nullptr;
        }
    }

    /// @brief The filepath template for the files.
    std::array<char, 512U> _filepath{};

    /// @brief The number of the next image to save.
    std::uint32_t _nextNumber{};

    /// @brief The value in which to increment the image number.
    std::uint32_t _increments{};

    /// @brief Pointer to the wrapped writer.
    TWrapped *_wrapped{};

    /// @brief The buffer for the wrapped writer.
    std::array<std::uint8_t, sizeof(TWrapped)> _buffer{};
};

} // namespace Terrahertz::ImageSeries

#endif // !THZ_IMAGE_IO_IMAGESERIESWRITER_HPP
