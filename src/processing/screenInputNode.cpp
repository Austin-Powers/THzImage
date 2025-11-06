#ifdef _WIN32

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

ScreenInputNode::ToCountResult ScreenInputNode::toCount(size_t const target) noexcept
{
    if (target < _buffer.count())
    {
        return ToCountResult::Ahead;
    }

    auto result = ToCountResult::NotUpdated;
    while (target > _buffer.count())
    {
        result = ToCountResult::Updated;
        if (!_buffer.next())
        {
            return ToCountResult::Failure;
        }
    }
    return result;
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

#endif // _WIN32
