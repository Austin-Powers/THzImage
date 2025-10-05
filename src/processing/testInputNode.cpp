#include "THzImage/processing/testInputNode.hpp"

#include "THzImage/io/testImageGenerator.hpp"

namespace Terrahertz::ImageProcessing {

TestInputNode::TestInputNode(Rectangle const &dimensions) noexcept
{
    TestImageGenerator generator{dimensions};
    (void)_image.readFrom(generator);
}

bool TestInputNode::next() noexcept { return true; }

TestInputNode::ImageType &TestInputNode::operator[](size_t const) noexcept { return _image; }

size_t TestInputNode::slots() const noexcept { return 1U; }

size_t TestInputNode::count() const noexcept { return 1U; }

} // namespace Terrahertz::ImageProcessing
