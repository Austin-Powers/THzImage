#include "THzImage/processing/imageInputNode.hpp"

namespace Terrahertz::ImageProcessing {

bool ImageInputNode::next(bool) noexcept
{
    ++_counter;
    return true;
}

ToCountResult ImageInputNode::toCount(size_t const target, bool) noexcept
{
    if (_counter > target)
    {
        return ToCountResult::Ahead;
    }
    else if (_counter == target)
    {
        return ToCountResult::NotUpdated;
    }
    _counter = target;
    return ToCountResult::Updated;
}

ImageInputNode::ImageType &ImageInputNode::operator[](size_t const) noexcept { return _image; }

size_t ImageInputNode::slots() const noexcept { return 1U; }

size_t ImageInputNode::count() const noexcept { return _counter; }

BGRAImage &ImageInputNode::image() noexcept { return _image; }

} // namespace Terrahertz::ImageProcessing
