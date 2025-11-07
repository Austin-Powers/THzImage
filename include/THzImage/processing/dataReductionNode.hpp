#ifndef THZ_IMAGE_PROCESSING_DATAREDUCTIONNODE_HPP
#define THZ_IMAGE_PROCESSING_DATAREDUCTIONNODE_HPP

#include "THzImage/processing/transformerNode.hpp"

#include <vector>

namespace Terrahertz::ImageProcessing {
namespace Internal {

class DataReductionTransformer : public IImageTransformer<MiniHSVPixel>
{
public:
    /// @brief Initializes a new DataReductionTransformer.
    ///
    /// @param node The node from which to pull the images.
    /// @param scaleFactor The factor by which to scale down the images.
    DataReductionTransformer(INode<BGRAPixel> &node, std::uint8_t const scaleFactor) noexcept;

    /// @copydoc IImageTransformer::dimensions
    Rectangle dimensions() const noexcept override;

    /// @copydoc IImageTransformer::transform
    bool transform(MyPixelType &) noexcept override;

    /// @copydoc IImageTransformer::skip
    bool skip() noexcept override;

    /// @copydoc IImageTransformer::reset
    bool reset() noexcept override;

    /// @copydoc IImageTransformer::nextImage
    bool nextImage() noexcept override;

private:
    void nextLine() noexcept;

    /// @brief The node from which to pull the images.
    INode<BGRAPixel> &_node;

    /// @brief The scale factor of data reduction.
    std::uint8_t _scaleFactor{};

    Rectangle _targetDimensions{};

    /// @brief Buffers an entire line of output pixels.
    std::vector<BGRAPixel> _lineBuffer{};
};

} // namespace Internal

/// @brief The actual node implementation.
using DataReductionNode = Internal::TransformerNode<MiniHSVPixel, Internal::DataReductionTransformer>;

} // namespace Terrahertz::ImageProcessing

#endif // !THZ_IMAGE_PROCESSING_DATAREDUCTIONNODE_HPP
