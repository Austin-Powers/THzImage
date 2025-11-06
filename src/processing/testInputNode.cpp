#include "THzImage/processing/testInputNode.hpp"

#include "THzImage/io/testImageGenerator.hpp"

namespace Terrahertz::ImageProcessing {

TestInputNode::TestInputNode(Rectangle const &dimensions) noexcept
{
    TestImageGenerator generator{dimensions};
    (void)_image.readFrom(generator);
}

bool TestInputNode::next() noexcept
{
    ++_counter;
    return true;
}

TestInputNode::ToCountResult TestInputNode::toCount(size_t const target) noexcept
{
    if (_counter > target)
    {
        return TestInputNode::ToCountResult::Ahead;
    }
    else if (_counter == target)
    {
        return TestInputNode::ToCountResult::NotUpdated;
    }
    _counter = target;
    return TestInputNode::ToCountResult::Updated;
}

TestInputNode::ImageType &TestInputNode::operator[](size_t const) noexcept { return _image; }

size_t TestInputNode::slots() const noexcept { return 1U; }

size_t TestInputNode::count() const noexcept { return _counter; }

} // namespace Terrahertz::ImageProcessing
