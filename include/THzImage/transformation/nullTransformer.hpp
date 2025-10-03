#ifndef THZ_IMAGE_HANDLING_NULLTRANSFORMER_HPP
#define THZ_IMAGE_HANDLING_NULLTRANSFORMER_HPP

#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/iImageTransformer.hpp"

namespace Terrahertz {

/// @brief Enables default construction of IImageTransformers
/// without the need for code in these classes handling default construction.
///
/// @tparam TPixelType The pixel type used by the transformer.
template <Pixel TPixelType>
class NullTransformer : public IImageTransformer<TPixelType>
{
public:
    /// @brief Returns an instance of the NullTransformer.
    ///
    /// @return An instance of the NullTransformer.
    static NullTransformer &instance() noexcept
    {
        static NullTransformer transformer{};
        return transformer;
    }

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
    /// @brief Initializes a new instance of the NullTransformer.
    NullTransformer() noexcept {}
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_HANDLING_NULLTRANSFORMER_HPP
