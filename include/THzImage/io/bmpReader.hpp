#ifndef THZ_IMAGE_IO_BMPREADER_HPP
#define THZ_IMAGE_IO_BMPREADER_HPP

#include "THzImage/common/iImageReader.hpp"
#include "THzImage/common/pixel.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>

namespace Terrahertz::BMP {

/// @brief Reads an image from a file using the BitMap format.
class Reader : public IImageReader<BGRAPixel>
{
public:
    /// @brief Initializes a new BMP::Reader.
    ///
    /// @param filepath The path of the file to read from.
    Reader(std::filesystem::path const filepath) noexcept;

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

    /// @brief Checks if the given file can be read as a BMP file.
    ///
    /// @return True if the file can be read, false otherwise.
    bool fileTypeFits() noexcept;

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
    /// @brief The stream from which to read the image data.
    std::ifstream _stream{};

    /// @brief The dimensions of the image.
    Rectangle _dimensions{};

    /// @brief The bit count of the image data.
    std::uint8_t _bitCount{};

    /// @brief The direction in which to read the lines of the file.
    bool _bottomUp{};
};

} // namespace Terrahertz::BMP

#endif // !THZ_IMAGE_IO_BMPREADER_HPP
