#ifndef THZ_IMAGE_IO_AUTOFILEREADER_HPP
#define THZ_IMAGE_IO_AUTOFILEREADER_HPP

#include "THzImage/common/iImageReader.hpp"

#include <filesystem>

namespace Terrahertz::AutoFile {

/// @brief File reader that automatically checks the file type and opens the correct one.
class Reader : public IImageReader<BGRAPixel>
{
public:
    /// @brief The size of the _innerReaderBuffer.
    static constexpr size_t InnerReaderBufferSize{600U};

    /// @brief The extension handling mode of the reader.
    enum class ExtensionMode
    {
        /// @brief Use extension as a hint but try all other supported formats.
        lenient,

        /// @brief Use only the format hinted by the extension.
        strict
    };

    using IImageReader::readInto;

    /// @brief Enabled default construction of the reader.
    Reader() noexcept = default;

    /// @brief Initializes a new AutoFile::Reader.
    ///
    /// @param path The path of the file to open.
    /// @param mode The extension handling mode of the reader.
    Reader(std::filesystem::path const path, ExtensionMode mode = ExtensionMode::lenient) noexcept;

    /// @brief Explicitly deleted to prevent copy construction.
    Reader(Reader const &other) noexcept = delete;

    /// @brief Explicitly deleted to prevent move construction.
    Reader(Reader &&other) noexcept = delete;

    /// @brief Explicitly deleted to prevent copy assignment.
    Reader &operator=(Reader const &other) noexcept = delete;

    /// @brief Explicitly deleted to prevent move assignment.
    Reader &operator=(Reader &&other) noexcept = delete;

    /// @brief Finalizes this instance, performing a deinit.
    ~Reader() noexcept;

    /// @brief Resets this AutoFile::Reader.
    ///
    /// @param path The path of the file to open.
    /// @param mode The extension handling mode of the reader.
    void reset(std::filesystem::path const path, ExtensionMode mode = ExtensionMode::lenient) noexcept;

    /// @copydoc IImageReader::imagePresent
    bool imagePresent() const noexcept override;

    /// @copydoc IImageReader::init
    bool init() noexcept override;

    /// @copydoc IImageReader::dimensions
    Rectangle dimensions() const noexcept override;

    /// @copydoc IImageReader::read
    bool read(gsl::span<BGRAPixel> buffer) noexcept override;

    /// @copydoc IImageReader::deinit
    void deinit() noexcept override;

private:
    /// @brief Deinitializes the inner reader.
    void deinitInnerReader() noexcept;

    /// @brief The path of the file to open.
    std::filesystem::path _path{};

    /// @brief The extension handling mode of the reader.
    ExtensionMode _mode{};

    /// @brief Pointer to the inner reader.
    IImageReader<BGRAPixel> *_innerReader{};

    /// @brief Memory to store the inner reader in.
    std::array<std::uint8_t, 2000U> _innerReaderBuffer{};
};

} // namespace Terrahertz::AutoFile

#endif // !THZ_IMAGE_IO_AUTOFILEREADER_HPP
