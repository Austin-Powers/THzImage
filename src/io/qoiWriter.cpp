#include "THzImage/io/qoiWriter.h"

#include "qoiCommons.h"

namespace Terrahertz::QOI {
namespace Internal {

Compressor::Compressor() noexcept { reset(); }

void Compressor::reset() noexcept
{
    _lastPixel = {};
    for (auto &color : _colorTable)
    {
        color = {};
    }
}

gsl::span<std::uint8_t const> Compressor::nextPixel(BGRAPixel const &pixel) noexcept
{
    if (_lastPixel == pixel)
    {
        return {};
    }
    auto const index  = pixelHash(pixel);
    auto       length = 0U;
    if (_colorTable[index] == pixel)
    {
        _codeBuffer[0U] = OpIndex | index;
        length          = 1U;
    }
    else if (_lastPixel.alpha == pixel.alpha)
    {
        _codeBuffer[0U] = OpRGB;
        _codeBuffer[1U] = pixel.red;
        _codeBuffer[2U] = pixel.green;
        _codeBuffer[3U] = pixel.blue;
        length          = 4U;
    }
    else
    {
        _codeBuffer[0U] = OpRGBA;
        _codeBuffer[1U] = pixel.red;
        _codeBuffer[2U] = pixel.green;
        _codeBuffer[3U] = pixel.blue;
        _codeBuffer[4U] = pixel.alpha;
        length          = 5U;
    }

    _lastPixel         = pixel;
    _colorTable[index] = pixel;
    return _codeSpan.subspan(0U, length);
}

} // namespace Internal

Writer::Writer(std::string_view const filepath) noexcept {}

bool Writer::init() noexcept { return true; }

bool Writer::write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept { return false; }

void Writer::deinit() noexcept {}

} // namespace Terrahertz::QOI
