#ifndef THZ_IMAGE_IO_IMAGEDIRECTORYREADER_HPP
#define THZ_IMAGE_IO_IMAGEDIRECTORYREADER_HPP

#include "THzImage/common/iImageReader.hpp"

#include <array>
#include <cstdint>
#include <filesystem>

namespace Terrahertz::ImageDirectory {

/// @brief Reads all images from a directory.
class Reader : public IImageReader<BGRAPixel>
{
public:
    /// @brief The operating mode of the reader.
    enum class Mode
    {
        /// @brief Try to open all files contained within the directory.
        /// @remark The file extension will be used for the first format tried on the file.
        /// @remark All known formats will be checked until one succeds.
        /// @remark If no format fits, the file will be skipped.
        automatic,

        /// @brief Only files with extensions of supported formats will be opened (BMP, PNG. QOI).
        /// @remark Only the format corresponding to the extension will be tried.
        /// @remark Files failing to open will be skipped.
        extensionBased,

        /// @brief Only files with extensions of supproted formats will be opened (BMP, PNG. QOI).
        /// @remark Only the format corresponding to the extension will be tried.
        /// @remark Loading will stop on first fail.
        strictExtensionBased
    };

    using IImageReader::readInto;

    /// @brief Initializes a new ImageDirectory::Reader.
    ///
    /// @param directorypath The path of the directory to read files from.
    /// @param mode The mode the reader is operating in.
    Reader(std::filesystem::path const directorypath, Mode const mode = Mode::strictExtensionBased) noexcept;

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

    /// @brief Returns the path of the last image loaded by the reader.
    ///
    /// @return The path of the last image loaded by the
    std::filesystem::path const &pathOfLastImage() const noexcept;

private:
    /// @brief The mode the reader is operating in.
    Mode _mode;

    /// @brief The iterator for the directory.
    std::filesystem::recursive_directory_iterator _iterator{};

    /// @brief The path of the last image loaded by the reader.
    std::filesystem::path _pathOfLastImage{};

    IImageReader *_innerReader{};

    // Find out how much the differnt readers use
    std::array<std::uint8_t, 40U> _innerReaderBuffer{};
};

} // namespace Terrahertz::ImageDirectory

#endif // !THZ_IMAGE_IO_IMAGEDIRECTORYREADER_HPP
