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
    using ExtensionMode = AutoFile::Reader::ExtensionMode;
    _iterator           = std::filesystem::recursive_directory_iterator{directorypath};
    _innerReaderMode    = (_mode == Mode::automatic) ? ExtensionMode::lenient : ExtensionMode::strict;
}

Reader::~Reader() noexcept { deinit(); }

bool Reader::imagePresent() const noexcept { return !(_iterator == std::filesystem::end(_iterator)); }

bool Reader::init() noexcept
{
    for (; imagePresent(); ++_iterator)
    {
        auto const &file = *_iterator;
        if (file.is_regular_file())
        {
            _pathOfLastImage = file.path();
            _innerReader.reset(_pathOfLastImage, _innerReaderMode);

            if (_innerReader.init())
            {
                ++_iterator;
                return true;
            }
            else if (_mode == Mode::strictExtensionBased)
            {
                if (_innerReader.extensionSupported())
                {
                    ++_iterator;
                    return false;
                }
            }
        }
    }
    return false;
}

Rectangle Reader::dimensions() const noexcept { return _innerReader.dimensions(); }

bool Reader::read(gsl::span<BGRAPixel> buffer) noexcept { return _innerReader.read(buffer); }

void Reader::deinit() noexcept { _innerReader.deinit(); }

std::filesystem::path const &Reader::pathOfLastImage() const noexcept { return _pathOfLastImage; }

} // namespace Terrahertz::ImageDirectory
