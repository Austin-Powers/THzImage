#ifndef THZ_IMAGE_IO_QOIWRITER_H
#define THZ_IMAGE_IO_QOIWRITER_H

#include "THzImage/common/iImageWriter.h"
#include "THzImage/common/pixel.h"

#include <array>
#include <gsl/gsl>
#include <string_view>

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
    gsl::span<std::uint8_t const> nextPixel(BGRAPixel const &pixel) noexcept;

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
    /// @brief Initializes a nwe BMPWriter.
    ///
    /// @param filepath The path to write the QOI-File to.
    Writer(std::string_view const filepath) noexcept;

    /// @copydoc IImageWriter::init
    bool init() noexcept override;

    /// @copydoc IImageWriter::write
    bool write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept override;

    /// @copydoc IImageWriter::deinit
    void deinit() noexcept override;

private:
    /// @brief The path to write the BMP-File to.
    std::string_view const _filepath;
};

} // namespace Terrahertz::QOI

#endif // !THZ_IMAGE_IO_BMPWRITER_H
