#include "THzImage/processing/screenInputNode.hpp"

namespace Terrahertz::ImageProcessing {

ScreenInputNode::ScreenInputNode(size_t const bufferSize) noexcept : _buffer{_reader, bufferSize} {}

void ScreenInputNode::setAreaToFullscreen() noexcept
{
    auto const screenDimensions = Screen::Reader::getScreenDimensions();
    (void)_reader.setArea(screenDimensions);
}

bool ScreenInputNode::setAreaTo(Rectangle const area) noexcept { return _reader.setArea(area); }

bool ScreenInputNode::next(bool const countFailure) noexcept { return _buffer.next(countFailure); }

ToCountResult ScreenInputNode::toCount(size_t const target, bool const force) noexcept
{
    if (target < _buffer.count())
    {
        return ToCountResult::Ahead;
    }
    if (target == _buffer.count())
    {
        return ToCountResult::NotUpdated;
    }
    while ((target - 1U) > _buffer.count())
    {
        _buffer.skip();
    }
    if (!_buffer.next(force) && !force)
    {
        return ToCountResult::Failure;
    }
    return ToCountResult::Updated;
}

ScreenInputNode::ImageType &ScreenInputNode::operator[](size_t const index) noexcept
{
    if (index < _buffer.slots())
    {
        return _buffer[index];
    }
    return _emptyImage;
}

size_t ScreenInputNode::slots() const noexcept { return _buffer.slots(); }

size_t ScreenInputNode::count() const noexcept { return _buffer.count(); }

} // namespace Terrahertz::ImageProcessing
