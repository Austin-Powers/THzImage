#include "THzImage/io/qoiReader.h"

#include "qoiCommons.h"

namespace Terrahertz::QOI {
namespace Internal {

Decompressor::Decompressor() noexcept {}

void Decompressor::setOutputBuffer(gsl::span<BGRAPixel> buffer) noexcept
{
    //
    _remainingImageBuffer = buffer;
}

size_t Decompressor::insertDataChunk(gsl::span<std::uint8_t const> const buffer) noexcept
{
    auto const storePixel = [this]() noexcept {
        _remainingImageBuffer[0U] = _lastPixel;
        _remainingImageBuffer     = _remainingImageBuffer.subspan(1U);
    };

    size_t readBytes = 0U;
    for (auto byte : buffer)
    {
        if (_remainingImageBuffer.empty())
        {
            break;
        }
        ++readBytes;
        switch (_nextByte)
        {
        case NextByte::Code: {
            if (byte == OpRGBA)
            {
                _nextByte = NextByte::RGBARed;
            }
            else if (byte == OpRGB)
            {
                _nextByte = NextByte::RGBRed;
            }
            break;
        }
        case NextByte::RGBARed: {
            _lastPixel.red = byte;
            _nextByte      = NextByte::RGBAGreen;
            break;
        }
        case NextByte::RGBAGreen: {
            _lastPixel.green = byte;
            _nextByte        = NextByte::RGBABlue;
            break;
        }
        case NextByte::RGBABlue: {
            _lastPixel.blue = byte;
            _nextByte       = NextByte::RGBAAlpha;
            break;
        }
        case NextByte::RGBAAlpha: {
            _lastPixel.alpha = byte;
            _nextByte        = NextByte::Code;
            storePixel();
            break;
        }
        case NextByte::RGBRed: {
            _lastPixel.red = byte;
            _nextByte      = NextByte::RGBGreen;
            break;
        }
        case NextByte::RGBGreen: {
            _lastPixel.green = byte;
            _nextByte        = NextByte::RGBBlue;
            break;
        }
        case NextByte::RGBBlue: {
            _lastPixel.blue = byte;
            _nextByte       = NextByte::Code;
            storePixel();
            break;
        }
        }
    }
    return readBytes;
}

} // namespace Internal
} // namespace Terrahertz::QOI
