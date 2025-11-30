#include "THzImage/processing/dataReductionNode.hpp"

namespace Terrahertz::ImageProcessing {

DataReductionNode::DataReductionNode(INode<BGRAPixel>  &node,
                                     std::uint8_t const scaleFactor,
                                     size_t const       bufferSize) noexcept
    : ReaderlessNodeBase(bufferSize), _node{node}, _scaleFactor{scaleFactor}
{}

bool DataReductionNode::prepareProcessing(size_t const count) noexcept
{
    auto const result = _node.toCount(count);
    if (_node[0U].dimensions().area() == 0U)
    {
        return false;
    }
    return (result == ToCountResult::NotUpdated) || (result == ToCountResult::Updated);
}

Rectangle DataReductionNode::dimensionsOfNextImage() const noexcept
{
    Rectangle dimensions = _node[0U].dimensions();
    dimensions.width /= _scaleFactor;
    dimensions.height /= _scaleFactor;
    return dimensions;
}

bool DataReductionNode::runProcessing(gsl::span<MiniHSVPixel> buffer) noexcept { return true; }

} // namespace Terrahertz::ImageProcessing
