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
    : _filepath{filepath}, _transparency{transparency}
{}

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

    auto const dataSize = 4U * dimensions.area();
    Header     header{};
    header.fileHeader.size      = sizeof(Header) + dataSize;
    header.infoHeader.bitCount  = 32U;
    header.infoHeader.width     = dimensions.width;
    header.infoHeader.height    = dimensions.height;
    header.infoHeader.sizeImage = dataSize;
    writeToStream(stream, header);
    writeToStream(stream, buffer);
    return true;
}

void Writer::deinit() noexcept {}

} // namespace Terrahertz::BMP
