#include "THzImage/processing/screenInputNode.hpp"

namespace Terrahertz::ImageProcessing {

ScreenInputNode::ScreenInputNode(size_t const bufferSize) noexcept : _buffer{_reader, bufferSize} {}

void ScreenInputNode::setAreaToFullscreen() noexcept
{
    auto const screenDimensions = Screen::Reader::getScreenDimensions();
    (void)_reader.setArea(screenDimensions);
}

bool ScreenInputNode::setAreaTo(Rectangle const area) noexcept { return _reader.setArea(area); }

bool ScreenInputNode::next() noexcept { return _buffer.next(); }

ScreenInputNode::ImageType &ScreenInputNode::operator[](size_t const index) noexcept
{
    if (index < _buffer.count())
    {
        return _buffer[index];
    }
    return _emptyImage;
}

} // namespace Terrahertz::ImageProcessing
