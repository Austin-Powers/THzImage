#ifndef THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMERBASE_HPP
#define THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMERBASE_HPP

#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/iImageTransformer.hpp"

#include <span>
#include <vector>

namespace Terrahertz {

/// @brief Base class for convolution based transformations.
///
/// @tparam TPixelType The type of pixel used by the transformation.
template <typename TPixelType>
class ConvolutionTransformerBase : public IImageTransformer<TPixelType>
{
public:
    ConvolutionTransformerBase(IImageTransformer<TPixelType> &base) noexcept {}

    /// @copydoc IImageTransformer::dimensions
    Rectangle dimensions() const noexcept override { return {}; }

    /// @copydoc IImageTransformer::transform
    bool transform(TPixelType &) noexcept override { return false; }

    /// @copydoc IImageTransformer::skip
    bool skip() noexcept override { return false; }

    /// @copydoc IImageTransformer::reset
    bool reset() noexcept override { return false; }

    /// @copydoc IImageTransformer::nextImage
    bool nextImage() noexcept override { return false; }

private:
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMERBASE_HPP
