#include "THzImage/io/qoiWriter.h"

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

    if (_lastPixel == pixel)
    {
        ++_run;
        if (_run == 63U)
        {
            _codeBuffer[0U] = OpRun | (_run - 1U);
            _run            = 0U;
            return _codeSpan.subspan(0U, 1U);
        }
        return {};
    }
    auto const index   = pixelHash(pixel);
    auto const deltaR  = channelDelta(pixel.red, _lastPixel.red);
    auto const deltaG  = channelDelta(pixel.green, _lastPixel.green);
    auto const deltaB  = channelDelta(pixel.blue, _lastPixel.blue);
    auto const deltaGR = deltaR - deltaG;
    auto const deltaGB = deltaB - deltaG;
    auto       length  = 0U;
    if (_colorTable[index] == pixel)
    {
        _codeBuffer[0U] = OpIndex | index;
        length          = 1U;
    }
    else if (_lastPixel.alpha == pixel.alpha)
    {
        if (inRange(deltaR, -2, 1) && inRange(deltaG, -2, 1) && inRange(deltaB, -2, 1))
        {
            _codeBuffer[0U] = OpDiff;
            _codeBuffer[0U] |= (deltaR + 2U) << 4U;
            _codeBuffer[0U] |= (deltaG + 2U) << 2U;
            _codeBuffer[0U] |= (deltaB + 2U);
            length = 1U;
        }
        else if (inRange(deltaG, -32, 31) && inRange(deltaGR, -8, 7) && inRange(deltaGB, -8, 7))
        {
            _codeBuffer[0U] = static_cast<std::uint8_t>(OpLuma | (deltaG + 32U));
            _codeBuffer[1U] = static_cast<std::uint8_t>((deltaGR + 8U) << 4U | (deltaGB + 8U));
            length          = 2U;
        }
        else
        {
            _codeBuffer[0U] = OpRGB;
            _codeBuffer[1U] = pixel.red;
            _codeBuffer[2U] = pixel.green;
            _codeBuffer[3U] = pixel.blue;
            length          = 4U;
        }
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
