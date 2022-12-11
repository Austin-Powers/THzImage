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
        _remainingImageBuffer[0U]          = _lastPixel;
        _remainingImageBuffer              = _remainingImageBuffer.subspan(1U);
        _colorTable[pixelHash(_lastPixel)] = _lastPixel;
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
            else
            {
                auto const code = byte & Mask2;
                if (code == OpIndex)
                {
                    _lastPixel = _colorTable[(byte & ~Mask2)];
                    storePixel();
                }
                else if (code == OpDiff)
                {
                    _lastPixel.red += ((byte >> 4U) & 0x03U) - 2;
                    _lastPixel.green += ((byte >> 2U) & 0x03U) - 2;
                    _lastPixel.blue += (byte & 0x03U) - 2;
                    storePixel();
                }
                else if (code == OpLuma)
                {
                    auto const delta = (byte & ~Mask2) - 32U;
                    _lastPixel.red += delta;
                    _lastPixel.green += delta;
                    _lastPixel.blue += delta;
                    _nextByte = NextByte::LumaByte2;
                }
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
        case NextByte::LumaByte2: {
            _lastPixel.red += ((byte >> 4U) & 0x0FU) - 8U;
            _lastPixel.blue += (byte & 0x0FU) - 8U;
            _nextByte = NextByte::Code;
            storePixel();
            break;
        }
        }
    }
    return readBytes;
}

} // namespace Internal
} // namespace Terrahertz::QOI
