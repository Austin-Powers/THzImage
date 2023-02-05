#ifndef THZ_IMAGE_IO_QOIWRITER_HPP
#define THZ_IMAGE_IO_QOIWRITER_HPP

#include "THzImage/common/iImageWriter.hpp"
#include "THzImage/common/pixel.hpp"

#include <array>
#include <filesystem>
#include <gsl/gsl>

namespace Terrahertz::QOI {
namespace Internal {

/// @brief Class containing the QOI compression algorithm for testing purposes.
class Compressor
{
public:
    /// @brief Initializes a new Compressor instance.
    Compressor() noexcept;

    /// @brief Resets the compressor to the initial state.
    void reset() noexcept;

    /// @brief Compresses the next pixel of the image.
    ///
    /// @param pixel The next pixel to compress.
    /// @return The next compressed bytes.
    [[nodiscard]] gsl::span<std::uint8_t const> nextPixel(BGRAPixel const &pixel) noexcept;

    /// @brief Flushes the buffer at the end of the image data.
    ///
    /// @return Either a final OpRun-Code or nothing.
    /// @remarks This will also reset the compressor.
    [[nodiscard]] gsl::span<std::uint8_t const> flush() noexcept;

private:
    /// @brief The last pixel saved.
    BGRAPixel _lastPixel{};

    /// @brief The current run length.
    std::uint8_t _run{};

    /// @brief The color table of the compressor.
    std::array<BGRAPixel, 64U> _colorTable{};

    /// @brief Buffer to assemble the next code to return.
    ///
    /// @remarks The worst case is a OpRun followed by a OpRGBA, which amounts to 6 bytes.
    std::array<std::uint8_t, 6U> _codeBuffer{};

    /// @brief The span of the _codeBuffer.
    gsl::span<std::uint8_t> _codeSpan{_codeBuffer};
};

} // namespace Internal

/// @brief Writes an image to a file using the Quite-Okay-Image format.
///
/// @remarks https://qoiformat.org/qoi-specification.pdf
class Writer : public IImageWriter<BGRAPixel>
{
public:
    /// @brief Initializes a new BMPWriter.
    ///
    /// @param filepath The path to write the QOI-File to.
    Writer(std::filesystem::path const filepath) noexcept;

    /// @copydoc IImageWriter::init
    bool init() noexcept override;

    /// @copydoc IImageWriter::write
    bool write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept override;

    /// @copydoc IImageWriter::deinit
    void deinit() noexcept override;

private:
    /// @brief The path to write the BMP-File to.
    std::filesystem::path const _filepath;
};

} // namespace Terrahertz::QOI

#endif // !THZ_IMAGE_IO_BMPWRITER_HPP
