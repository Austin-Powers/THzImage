#include "THzImage/common/displayserver.hpp"
#ifdef THZ_IMAGE_DUMMY_SCREENREADER_USED

#include "THzImage/io/screenReader.hpp"

#include "THzCommon/math/rectangle.hpp"
namespace Terrahertz::Screen {

struct Reader::Impl
{};

Reader::Reader() noexcept {}

Reader::Reader(Rectangle const &area) noexcept {}

Reader::~Reader() noexcept {}

bool Reader::setArea(Rectangle const &area) noexcept { return true; }

bool Reader::imagePresent() const noexcept { return true; }

bool Reader::init() noexcept { return true; }

Rectangle Reader::dimensions() const noexcept { return Rectangle{1U, 1U}; }

bool Reader::read(gsl::span<BGRAPixel>) noexcept { return true; }

void Reader::deinit() noexcept {}

} // namespace Terrahertz::Screen

#endif // THZ_IMAGE_DUMMY_SCREENREADER_USED
