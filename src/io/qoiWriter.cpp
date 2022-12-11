#include "THzImage/io/qoiWriter.h"

#include "THzCommon/logging/logging.h"
#include "THzCommon/utility/fstreamhelpers.h"
#include "qoiCommons.h"

#include <THzCommon/math/inrange.h>

namespace Terrahertz::QOI {
namespace Internal {

Compressor::Compressor() noexcept { reset(); }

void Compressor::reset() noexcept
{
    _lastPixel = BGRAPixel{};
    for (auto &color : _colorTable)
    {
        color = BGRAPixel{};
    }
}

gsl::span<std::uint8_t const> Compressor::nextPixel(BGRAPixel const &pixel) noexcept
{
    auto const channelDelta = [](std::uint8_t is, std::uint8_t was) noexcept -> std::int32_t {
        auto const d = is - was;
        if (d >= 128)
        {
            // this handles diff values if over-/underflow is involved
            is += 128;
            was += 128;
            return is - was;
        }
        return d;
    };
    auto       length        = 0U;
    auto const writeToBuffer = [&](std::uint32_t const byte) noexcept {
        _codeBuffer[length] = static_cast<std::uint8_t>(byte);
        ++length;
    };
    auto const writeRun = [&]() noexcept {
        writeToBuffer(OpRun | (_run - 1U));
        _run = 0U;
    };

    if (_lastPixel == pixel)
    {
        if (++_run == 62U)
        {
            writeRun();
            return _codeSpan.subspan(0U, 1U);
        }
        return {};
    }

    if (_run > 0U)
    {
        writeRun();
    }
    auto const index   = pixelHash(pixel);
    auto const deltaR  = channelDelta(pixel.red, _lastPixel.red);
    auto const deltaG  = channelDelta(pixel.green, _lastPixel.green);
    auto const deltaB  = channelDelta(pixel.blue, _lastPixel.blue);
    auto const deltaGR = deltaR - deltaG;
    auto const deltaGB = deltaB - deltaG;
    if (_colorTable[index] == pixel)
    {
        writeToBuffer(OpIndex | index);
    }
    else if (_lastPixel.alpha == pixel.alpha)
    {
        if (inRange(deltaR, -2, 1) && inRange(deltaG, -2, 1) && inRange(deltaB, -2, 1))
        {
            writeToBuffer(OpDiff | ((deltaR + 2U) << 4U) | ((deltaG + 2U) << 2U) | (deltaB + 2U));
        }
        else if (inRange(deltaG, -32, 31) && inRange(deltaGR, -8, 7) && inRange(deltaGB, -8, 7))
        {
            writeToBuffer(OpLuma | (deltaG + 32U));
            writeToBuffer(((deltaGR + 8U) << 4U) | (deltaGB + 8U));
        }
        else
        {
            writeToBuffer(OpRGB);
            writeToBuffer(pixel.red);
            writeToBuffer(pixel.green);
            writeToBuffer(pixel.blue);
        }
    }
    else
    {
        writeToBuffer(OpRGBA);
        writeToBuffer(pixel.red);
        writeToBuffer(pixel.green);
        writeToBuffer(pixel.blue);
        writeToBuffer(pixel.alpha);
    }

    _lastPixel         = pixel;
    _colorTable[index] = pixel;
    return _codeSpan.subspan(0U, length);
}

gsl::span<std::uint8_t const> Compressor::flush() noexcept
{
    auto const run = _run;
    reset();
    if (run > 0U)
    {
        _codeBuffer[0U] = (OpRun | (run - 1U));
        return _codeSpan.subspan(0U, 1U);
    }
    return {};
}

} // namespace Internal

/// @brief Name provider for the THzImage.IO.QOIWriter class.
struct WriterProject
{
    static constexpr char const *name() noexcept { return "THzImage.IO.QOIWriter"; }
};

Writer::Writer(std::string_view const filepath) noexcept : _filepath{filepath} {}

bool Writer::init() noexcept { return true; }

bool Writer::write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept
{
    if (dimensions.area() != buffer.size())
    {
        logMessage<LogLevel::Error, WriterProject>("Image dimensions do not match the given buffer size");
        return false;
    }

    // As string_view is not zero terminated, we copy it just to be save when opening the stream.
    std::array<char, 512U> filepath{};
    std::memcpy(filepath.data(), _filepath.data(), std::min(filepath.size(), _filepath.size()));

    std::ofstream stream{filepath.data(), std::ios::binary};
    if (!stream.is_open())
    {
        logMessage<LogLevel::Error, WriterProject>("Could not open the file to write");
        return false;
    }
    Header header{};
    header.magic      = Header::MagicBytes;
    header.width      = dimensions.width;
    header.height     = dimensions.height;
    header.channels   = 4U;
    header.colorspace = 0U;
    writeToStream(stream, header);
    Internal::Compressor compressor{};
    for (auto const &pixel : buffer)
    {
        writeToStream(stream, compressor.nextPixel(pixel));
    }
    writeToStream(stream, compressor.flush());
    return true;
}

void Writer::deinit() noexcept {}

} // namespace Terrahertz::QOI
