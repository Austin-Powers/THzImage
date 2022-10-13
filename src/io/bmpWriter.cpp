#include "THzImage/io/bmpWriter.h"

#include "THzCommon/utility/fstreamhelpers.h"
#include "bmpCommons.h"

#include <cstring>
#include <fstream>

namespace Terrahertz::BMP {

Writer::Writer(std::string_view const filepath, bool const transparency) noexcept
    : _filepath{filepath}, _transparency{transparency}
{}

bool Writer::init() noexcept { return true; }

bool Writer::write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept
{
    if (dimensions.area() != buffer.size())
    {
        return false;
    }

    // As string_view is not zero terminated, we copy it just to be save when opening the stream.
    std::array<char, 512U> filepath{};
    std::memcpy(filepath.data(), _filepath.data(), std::min(filepath.size(), _filepath.size()));

    std::ofstream stream{filepath.data(), std::ios::binary};
    if (!stream.is_open())
    {
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
