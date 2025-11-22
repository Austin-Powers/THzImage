#include "THzImage/processing/dataReductionNode.hpp"

#ifdef CURRENTLY_NOT_IMPLEMENTED

namespace Terrahertz::ImageProcessing::Internal {

DataReductionTransformer::DataReductionTransformer(INode<BGRAPixel> &node, std::uint8_t const scaleFactor) noexcept
    : _node{node}, _scaleFactor{scaleFactor}
{
    if (node.count() == 0U)
    {}
}

Rectangle DataReductionTransformer::dimensions() const noexcept { return _targetDimensions; }

bool DataReductionTransformer::transform(MyPixelType &) noexcept { return true; }

bool DataReductionTransformer::skip() noexcept { return true; }

bool DataReductionTransformer::reset() noexcept { return true; }

bool DataReductionTransformer::nextImage() noexcept
{
    auto const result = _node.next();
    if (result)
    {
        auto const &imageDimensions = _node[0U].dimensions();
        _targetDimensions = Rectangle{imageDimensions.width / _scaleFactor, imageDimensions.height / _scaleFactor};
    }
    return result;
}

} // namespace Terrahertz::ImageProcessing::Internal

#endif
