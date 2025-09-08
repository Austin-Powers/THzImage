#include "THzImage/io/imageDirectoryReader.hpp"

#include "THzCommon/logging/logging.hpp"

namespace Terrahertz::ImageDirectory {

/// @brief Name provider for the THzImage.IO.ImageDirectory.Reader class.
struct ReaderProject
{
    static constexpr char const *name() noexcept { return "THzImage.IO.ImageDirectory.Reader"; }
};

Reader::Reader(std::filesystem::path const directorypath, Reader::Mode const mode) noexcept : _mode{mode}
{
    _iterator = std::filesystem::recursive_directory_iterator{directorypath};
}

Reader::~Reader() noexcept { deinit(); }

bool Reader::imagePresent() const noexcept { return !(_iterator == std::filesystem::end(_iterator)); }

bool Reader::init() noexcept
{
    // while (imagePresent())
    {
        // iterate till file found
        auto const file = *_iterator;
        // create reader
        // call init on reader
    }
    return false;
}

Rectangle Reader::dimensions() const noexcept { return {}; }

bool Reader::read(gsl::span<BGRAPixel> buffer) noexcept { return false; }

void Reader::deinit() noexcept {}

std::filesystem::path const &Reader::pathOfLastImage() const noexcept { return _pathOfLastImage; }

} // namespace Terrahertz::ImageDirectory
