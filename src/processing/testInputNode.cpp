#include "THzImage/processing/testInputNode.hpp"

#include "THzImage/io/testImageGenerator.hpp"

namespace Terrahertz::ImageProcessing {

TestInputNode::TestInputNode(Rectangle const &dimensions) noexcept
{
    TestImageGenerator generator{dimensions};
    (void)_image.readFrom(generator);
}

bool TestInputNode::next(bool) noexcept
{
    ++_counter;
    return true;
}

ToCountResult TestInputNode::toCount(size_t const target, bool) noexcept
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

TestInputNode::ImageType &TestInputNode::operator[](size_t const) noexcept { return _image; }

size_t TestInputNode::slots() const noexcept { return 1U; }

size_t TestInputNode::count() const noexcept { return _counter; }

} // namespace Terrahertz::ImageProcessing
