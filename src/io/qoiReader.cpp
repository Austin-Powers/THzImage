#include "THzImage/io/qoiReader.h"

#include "THzCommon/logging/logging.h"
#include "THzCommon/utility/fstreamhelpers.h"
#include "THzCommon/utility/spanhelpers.h"
#include "qoiCommons.h"

namespace Terrahertz::QOI {
namespace Internal {

Decompressor::Decompressor() noexcept {}

void Decompressor::setOutputBuffer(gsl::span<BGRAPixel> buffer) noexcept
{
    _nextByte             = NextByte::Code;
    _lastPixel            = BGRAPixel{};
    _remainingImageBuffer = buffer;
}

size_t Decompressor::insertDataChunk(gsl::span<std::uint8_t const> const buffer) noexcept
{
    auto const storePixel = [this]() noexcept {
        if (!_remainingImageBuffer.empty())
        {
            _remainingImageBuffer[0U]          = _lastPixel;
            _remainingImageBuffer              = _remainingImageBuffer.subspan(1U);
            _colorTable[pixelHash(_lastPixel)] = _lastPixel;
        }
    };

    size_t readBytes = 0U;
    for (auto byte : buffer)
    {
        if (_remainingImageBuffer.empty())
        {
            break;
        }
        switch (_nextByte)
        {
        case NextByte::Code: {
            auto const code = byte & Mask2;
            if (byte == OpRGBA)
            {
                _nextByte = NextByte::RGBARed;
            }
            else if (byte == OpRGB)
            {
                _nextByte = NextByte::RGBRed;
            }
            else if (code == OpIndex)
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
            else if (code == OpRun)
            {
                for (auto i = (byte & ~Mask2) + 1U; i != 0; --i)
                {
                    storePixel();
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
        ++readBytes;
    }
    return readBytes;
}

} // namespace Internal

/// @brief Name provider for the THzImage.IO.QOIReader class.
struct ReaderProject
{
    static constexpr char const *name() noexcept { return "THzImage.IO.QOI.Reader"; }
};

Reader::Reader(std::string_view const filepath) noexcept
{
    Logger::globalInstance().addProject<ReaderProject>();
    // As string_view is not zero terminated, we copy it just to be save when opening the stream.
    std::array<char, 512U> path{};
    std::memcpy(path.data(), filepath.data(), std::min(path.size(), filepath.size()));
    _stream.open(path.data(), std::ios::binary);
}

Reader::~Reader() noexcept { deinit(); }

bool Reader::multipleImages() const noexcept { return false; }

bool Reader::init() noexcept
{
    if (!_stream.is_open())
    {
        logMessage<LogLevel::Error, ReaderProject>("File could not be opened");
        return false;
    }
    Header header{};
    if (!readFromStream(_stream, header))
    {
        logMessage<LogLevel::Error, ReaderProject>("File too small for header structure");
        return false;
    }
    if (header.magic != Header::MagicBytes)
    {
        logMessage<LogLevel::Error, ReaderProject>("Given file is not a QOI file");
        return false;
    }
    if (header.width == 0)
    {
        logMessage<LogLevel::Error, ReaderProject>("Width is zero");
        return false;
    }
    if (header.height == 0)
    {
        logMessage<LogLevel::Error, ReaderProject>("Height is zero");
        return false;
    }
    _dimensions.width  = header.width;
    _dimensions.height = header.height;
    return true;
}

Rectangle Reader::dimensions() const noexcept { return _dimensions; }

bool Reader::read(gsl::span<BGRAPixel> buffer) noexcept
{
    std::array<std::uint8_t, 256U> readBuffer{};
    Internal::Decompressor         decompressor{};
    decompressor.setOutputBuffer(buffer);

    auto readSpan = toSpan<std::uint8_t>(readBuffer);
    for (auto bytes = readFromStream(_stream, readBuffer); bytes != 0U; bytes = readFromStream(_stream, readBuffer))
    {
        if (decompressor.insertDataChunk(readSpan.subspan(0U, bytes)) != bytes)
        {
            logMessage<LogLevel::Error, ReaderProject>("Ran out of image buffer space");
            return false;
        }
    }
    return true;
}

void Reader::deinit() noexcept { _stream.close(); }

} // namespace Terrahertz::QOI
