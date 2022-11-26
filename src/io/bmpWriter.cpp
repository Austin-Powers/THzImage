#include "THzImage/io/bmpWriter.h"

#include "THzCommon/logging/logging.h"
#include "THzCommon/utility/fstreamhelpers.h"
#include "bmpCommons.h"

#include <cstring>
#include <fstream>

namespace Terrahertz::BMP {

/// @brief Name provider for the THzImage.IO.BMPWriter class.
struct WriterProject
{
    static constexpr char const *name() noexcept { return "THzImage.IO.BMPWriter"; }
};

Writer::Writer(std::string_view const filepath, bool const transparency) noexcept
    : _filepath{filepath}, _bitCount{transparency ? std::uint8_t{32U} : std::uint8_t{24U}}
{
    Logger::globalInstance().addProject<WriterProject>();
}

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

    Header header{static_cast<std::int32_t>(dimensions.width), static_cast<std::int32_t>(dimensions.height), _bitCount};
    writeToStream(stream, header);
    if (_bitCount == 32U)
    {
        writeToStream(stream, buffer);
    }
    else
    {
        auto const bytesUsed  = (header.infoHeader.bitCount / 8U) * header.infoHeader.width;
        auto const lineLength = (bytesUsed + 3U) & ~3U;
        auto const padding    = lineLength - bytesUsed;

        std::array<char, 3U> const paddingBytes{};

        auto x = 0U;
        for (auto const &pixel : buffer)
        {
            stream.write(std::bit_cast<char const *>(&pixel), 3U);
            ++x;
            if (x == header.infoHeader.width)
            {
                x = 0U;
                stream.write(paddingBytes.data(), padding);
            }
        }
    }

    return true;
}

void Writer::deinit() noexcept {}

} // namespace Terrahertz::BMP
