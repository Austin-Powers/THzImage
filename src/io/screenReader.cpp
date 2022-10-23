#ifdef _WIN32

#include "THzImage/io/screenReader.h"

#include <Windows.h>
#include <cstdint>

namespace Terrahertz::Screen {

Rectangle Reader::getScreenDimensions() noexcept
{
    return Rectangle{static_cast<std::uint32_t>(GetSystemMetrics(SM_CXSCREEN)),
                     static_cast<std::uint32_t>(GetSystemMetrics(SM_CYSCREEN))};
}

/// @brief Implementation of the Reader.
struct Reader::Impl
{
    Impl(Rectangle const &area) noexcept : _area{area} {}

    Rectangle area() const noexcept { return _area; }

    Rectangle _area{};
};

Reader::Reader() noexcept { _impl = std::make_unique<Reader::Impl>(getScreenDimensions()); }

Reader::Reader(Rectangle const &area) noexcept { _impl = std::make_unique<Reader::Impl>(area); }

Reader::~Reader() noexcept
{
    deinit();
    _impl.reset();
}

bool Reader::setArea(Rectangle const &) noexcept { return false; }

bool Reader::multipleImages() const noexcept { return true; }

bool Reader::init() noexcept { return false; }

Rectangle Reader::dimensions() const noexcept { return _impl->area(); }

bool Reader::read(gsl::span<BGRAPixel>) noexcept { return false; }

void Reader::deinit() noexcept {}

} // namespace Terrahertz::Screen

#endif // !_WIN32
