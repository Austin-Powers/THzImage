#ifndef THZ_IMAGE_PROCESSING_DATAREDUCTIONNODE_HPP
#define THZ_IMAGE_PROCESSING_DATAREDUCTIONNODE_HPP

#include "THzImage/processing/readerlessNodeBase.hpp"

namespace Terrahertz::ImageProcessing {

class DataReductionNode : public ReaderlessNodeBase<MiniHSVPixel>
{
public:
    /// @brief Initializes a new DataReductionNode.
    ///
    /// @param node The node from which to pull the images.
    /// @param scaleFactor The factor by which to scale down the images.
    /// @param bufferSize The amount of image sthe node will store.
    DataReductionNode(INode<BGRAPixel> &node, std::uint8_t const scaleFactor, size_t const bufferSize) noexcept;

private:
    /// @copydoc ReaderlessNodeBase::prepareProcessing
    bool prepareProcessing(size_t const count) noexcept override;

    /// @copydoc ReaderlessNodeBase::dimensionsOfNextImage
    Rectangle dimensionsOfNextImage() const noexcept override;

    /// @copydoc ReaderlessNodeBase::runProcessing
    bool runProcessing(gsl::span<MiniHSVPixel> buffer) noexcept override;

    /// @brief The node from which to pull the images.
    INode<BGRAPixel> &_node;

    /// @brief The scale factor of data reduction.
    std::uint8_t _scaleFactor{};
};

} // namespace Terrahertz::ImageProcessing

#endif // !THZ_IMAGE_PROCESSING_DATAREDUCTIONNODE_HPP
