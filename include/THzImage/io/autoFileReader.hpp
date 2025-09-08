#ifndef THZ_IMAGE_IO_AUTOFILEREADER_HPP
#define THZ_IMAGE_IO_AUTOFILEREADER_HPP

#include "THzImage/common/iImageReader.hpp"

#include <filesystem>

namespace Terrahertz::AutoFile {

/// @brief File reader that automatically checks the file type and opens the correct one.
class Reader : public IImageReader<BGRAPixel>
{
public:
    using IImageReader::readInto;

    /// @brief Enabled default construction of the reader.
    Reader() noexcept = default;

    /// @brief Initializes a new AutoFile::Reader.
    ///
    /// @param path The path of the file to open.
    /// @param extensionOnly True if the reader shall only try the format hinted by the file extension,
    /// false to check every known format starting with the one hinted at by the extension.
    Reader(std::filesystem::path const path, bool extensionOnly) noexcept;

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
    /// @param extensionOnly True if the reader shall only try the format hinted by the file extension,
    /// false to check every known format starting with the one hinted at by the extension.
    void reset(std::filesystem::path const path, bool extensionOnly) noexcept;

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
    /// @brief The path of the file to open.
    std::filesystem::path _path{};

    /// @brief True if the reader shall only check the format hinted at by the file extension.
    bool _extensionOnly{};
};

} // namespace Terrahertz::AutoFile

#endif // !THZ_IMAGE_IO_AUTOFILEREADER_HPP
