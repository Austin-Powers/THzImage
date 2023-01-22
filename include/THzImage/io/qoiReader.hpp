#ifndef THZ_IMAGE_IO_QOIREADER_HPP
#define THZ_IMAGE_IO_QOIREADER_HPP

#include "THzImage/common/iImageReader.hpp"
#include "THzImage/common/pixel.hpp"

#include <cstdint>
#include <fstream>
#include <gsl/gsl>
#include <string_view>

namespace Terrahertz::QOI {
namespace Internal {

/// @brief Class containing the QOI decompression algorithm for testing purposes.
class Decompressor
{
public:
    /// @brief Enum class for signalling how to interpret the next byte to read.
    enum class NextByte
    {
        Code,
        RGBARed,
        RGBAGreen,
        RGBABlue,
        RGBAAlpha,
        RGBRed,
        RGBGreen,
        RGBBlue,
        LumaByte2
    };

    /// @brief Initializes a new Decompressor instance.
    Decompressor() noexcept;

    /// @brief Sets the buffer for the decompressed image data.
    ///
    /// @param buffer The buffer for the decompressed data.
    /// @remarks This will also reset the decompressor.
    void setOutputBuffer(gsl::span<BGRAPixel> buffer) noexcept;

    /// @brief Inserts the next chunk of compressed image data to decompress.
    ///
    /// @param buffer The buffer containing the compressed data.
    /// @return The amount of bytes read from the buffer.
    /// @remarks Reading will stop once the given buffer for the image data is full.
    size_t insertDataChunk(gsl::span<std::uint8_t const> const buffer) noexcept;

private:
    /// @brief The remaining image buffer to put the decoded data into.
    gsl::span<BGRAPixel> _remainingImageBuffer{};

    /// @brief The last pixel read.
    BGRAPixel _lastPixel{};

    /// @brief The color table of the compressor.
    std::array<BGRAPixel, 64U> _colorTable{};

    /// @brief The value telling the decompressor how to interpret the next byte.
    NextByte _nextByte{NextByte::Code};
};

} // namespace Internal

/// @brief Reads an image from a file using the Quite-Okay-Image format.
///
/// @remarks https://qoiformat.org/qoi-specification.pdf
class Reader : public IImageReader<BGRAPixel>
{
public:
    /// @brief Initializes a new QOI::Reader.
    ///
    /// @param filepath The path of the file to read from.
    Reader(std::string_view const filepath) noexcept;

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

    /// @brief Checks if the given file can be read as a QOI file.
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
    /// @brief The stream from which to read the iamge data.
    std::ifstream _stream{};

    /// @brief The dimensions of the image.
    Rectangle _dimensions{};

    /// @brief The decompressor for the image data.
    Internal::Decompressor _decompressor{};
};

} // namespace Terrahertz::QOI

#endif // !THZ_IMAGE_IO_QOIREADER_HPP
