#include "THzImage/io/bmpReader.hpp"

#include "THzCommon/logging/logging.hpp"
#include "THzCommon/utility/fstreamhelpers.hpp"
#include "THzCommon/utility/lineSequencer.hpp"
#include "bmpCommons.hpp"

#include <array>
#include <cstring>

namespace Terrahertz::BMP {

/// @brief Name provider for the THzImage.IO.BMPReader class.
struct ReaderProject
{
    static constexpr char const *name() noexcept { return "THzImage.IO.BMP.Reader"; }
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
    if (header.fileHeader.magic != FileHeader::MagicBytes)
    {
        logMessage<LogLevel::Error, ReaderProject>("Given file is not a BMP file");
        return false;
    }
    if ((header.infoHeader.bitCount != 24U) && (header.infoHeader.bitCount != 32U))
    {
        logMessage<LogLevel::Error, ReaderProject>("Unsupported bitCount");
        return false;
    }
    if (header.infoHeader.compression != 0U)
    {
        logMessage<LogLevel::Error, ReaderProject>("Unsupported compression");
        return false;
    }
    if (header.infoHeader.width == 0)
    {
        logMessage<LogLevel::Error, ReaderProject>("Width is zero");
        return false;
    }
    if (header.infoHeader.height == 0)
    {
        logMessage<LogLevel::Error, ReaderProject>("Height is zero");
        return false;
    }
    if (header.infoHeader.compression != 0U)
    {
        logMessage<LogLevel::Error, ReaderProject>("Unsupported compression");
        return false;
    }
    if (header.fileHeader.offBits != 54U)
    {
        logMessage<LogLevel::Error, ReaderProject>("Unsupported number of offBits");
        return false;
    }
    _bottomUp = true;
    if (header.infoHeader.height < 0)
    {
        header.infoHeader.height = -header.infoHeader.height;
        _bottomUp                = false;
    }
    Header sanityCheck{header.infoHeader.width, header.infoHeader.height, header.infoHeader.bitCount};
    if (header.infoHeader.sizeImage != sanityCheck.infoHeader.sizeImage)
    {
        logMessage<LogLevel::Error, ReaderProject>("Size of the image data does not match dimensions and bitcount");
        return false;
    }
    _bitCount          = static_cast<std::uint8_t>(header.infoHeader.bitCount);
    _dimensions.width  = header.infoHeader.width;
    _dimensions.height = header.infoHeader.height;
    return true;
}

Rectangle Reader::dimensions() const noexcept { return _dimensions; }

bool Reader::read(gsl::span<BGRAPixel> buffer) noexcept
{
    if (buffer.size() < _dimensions.area())
    {
        logMessage<LogLevel::Error, ReaderProject>("Given buffer is too small for the data");
        return false;
    }
    auto sequencer = LineSequencer<BGRAPixel>::create(buffer, _dimensions.width, _bottomUp);
    if (_bitCount == 32U)
    {
        for (auto line = sequencer->nextLine(); !line.empty(); line = sequencer->nextLine())
        {
            if (readFromStream(_stream, line) != line.size())
            {
                return false;
            }
        }
    }
    else
    {
        auto const bytesUsed  = (_bitCount / 8U) * _dimensions.width;
        auto const lineLength = (bytesUsed + 3U) & ~3U;
        auto const padding    = lineLength - bytesUsed;

        std::array<char, 3U> paddingBytes{};

        for (auto line = sequencer->nextLine(); !line.empty(); line = sequencer->nextLine())
        {
            for (auto const &pixel : line)
            {
                _stream.read(std::bit_cast<char *>(&pixel), 3U);
                if (!_stream.good())
                {
                    return false;
                }
            }
            _stream.read(paddingBytes.data(), padding);
            if (!_stream.good())
            {
                return false;
            }
        }
    }
    return true;
}

void Reader::deinit() noexcept { _stream.close(); }

} // namespace Terrahertz::BMP
