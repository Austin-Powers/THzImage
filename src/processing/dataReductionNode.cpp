#include "THzImage/processing/dataReductionNode.hpp"

#include <utility>

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
    _dimensionsOfNextImage = _node[0U].dimensions();
    _dimensionsOfNextImage.width /= _scaleFactor;
    _dimensionsOfNextImage.height /= _scaleFactor;
    return (result == ToCountResult::NotUpdated) || (result == ToCountResult::Updated);
}

Rectangle DataReductionNode::dimensionsOfNextImage() const noexcept { return _dimensionsOfNextImage; }

bool DataReductionNode::runProcessing(gsl::span<MiniHSVPixel> buffer) noexcept
{
    auto const updatePixel = [](BGRAPixel &target, BGRAPixel other) {
        target.blue  = std::max(target.blue, other.blue);
        target.green = std::max(target.green, other.green);
        target.red   = std::max(target.red, other.red);
    };

    auto const &baseImage = _node[0U];
    if (_dimensionsOfNextImage.area() > buffer.size())
    {
        return false;
    }
    if (_bins.size() < _dimensionsOfNextImage.width)
    {
        _bins.resize(_dimensionsOfNextImage.width);
    }
    std::uint32_t const lineRemainder = baseImage.dimensions().width % _scaleFactor;

    size_t sourceIndex = 0U;
    for (auto yT = 0U; yT < _dimensionsOfNextImage.height; ++yT)
    {
        auto const lineOffset = static_cast<size_t>(_dimensionsOfNextImage.width) * yT;
        // first line: set values
        for (auto xT = 0U; xT < _dimensionsOfNextImage.width; ++xT)
        {
            _bins[xT] = baseImage[sourceIndex++];
            for (auto xS = 1U; xS < _scaleFactor; ++xS)
            {
                updatePixel(_bins[xT], baseImage[sourceIndex++]);
            }
        }
        sourceIndex += lineRemainder;
        // middle lines: update values
        for (auto yS = 1U; yS < (_scaleFactor - 1U); ++yS)
        {
            for (auto xT = 0U; xT < _dimensionsOfNextImage.width; ++xT)
            {
                for (auto xS = 0U; xS < _scaleFactor; ++xS)
                {
                    updatePixel(_bins[xT], baseImage[sourceIndex++]);
                }
            }
            sourceIndex += lineRemainder;
        }
        // last line: transfer to buffer
        for (auto xT = 0U; xT < _dimensionsOfNextImage.width; ++xT)
        {
            for (auto xS = 0U; xS < _scaleFactor; ++xS)
            {
                updatePixel(_bins[xT], baseImage[sourceIndex++]);
            }
            buffer[lineOffset + xT] = _bins[xT];
        }
        sourceIndex += lineRemainder;
    }
    return true;
}

} // namespace Terrahertz::ImageProcessing
