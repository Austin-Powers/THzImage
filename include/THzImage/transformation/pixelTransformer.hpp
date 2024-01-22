#ifndef THZ_IMAGE_TRANSFORMATION_PIXELTRANSFORMER_HPP
#define THZ_IMAGE_TRANSFORMATION_PIXELTRANSFORMER_HPP

#include "THzImage/common/iImageTransformer.hpp"

#include <concepts>

namespace Terrahertz {

// clang-format off

/// @brief Concept of a class transforming pixels of TPixelType.
template <typename TType, typename TPixelType>
concept PixelTransformation = requires(TType t, TPixelType pixel)
{
    {t(pixel)} -> std::same_as<TPixelType>;
};

// clang-format on

/// @brief Class wrapping Pixel-to-Pixel transformation algorithms
///        to make them implement the IImageTransformer interface.
///
/// @tparam TPixelType The type of pixel transformed.
/// @tparam TTransformation The type of the transformation class.
template <typename TPixelType, PixelTransformation<TPixelType> TTransformation>
class PixelTransformer : public IImageTransformer<TPixelType>
{
public:
    /// @brief Initializes a new PixelTransformer using the given values.
    ///
    /// @param wrapped The previous transformer in the chain to wrap.
    /// @param transformation The instance encapsulating the transformation algorithm.
    PixelTransformer(IImageTransformer<TPixelType> &wrapped, TTransformation transformation) noexcept
        : _wrapped{wrapped}, _transformation{transformation}
    {}

    /// @copydoc IImageTransformer::dimensions
    Rectangle dimensions() const noexcept override { return _wrapped.dimensions(); }

    /// @copydoc IImageTransformer::transform
    bool transform(TPixelType &pixel) noexcept override
    {
        auto const result = _wrapped.transform(pixel);
        pixel             = _transformation(pixel);
        return result;
    }

    /// @copydoc IImageTransformer::skip
    bool skip() noexcept override { return _wrapped.skip(); }

    /// @copydoc IImageTransformer::reset
    bool reset() noexcept override { return _wrapped.reset(); }

    /// @copydoc IImageTransformer::nextImage
    bool nextImage() noexcept override { return _wrapped.nextImage(); }

private:
    /// @brief The previous transformer in the chain to wrap.
    IImageTransformer<TPixelType> &_wrapped;

    /// @brief The instance encapsulating the transformation algorithm.
    TTransformation _transformation;
};

/// @brief Helper method to ease the creation of PixelTransformers.
///
/// @tparam TPixelType The pixel type used by the transformer.
/// @tparam TTransformation The type of transformation.
/// @param transformation The transformation instance.
/// @return The created transformer.
template <typename TPixelType, PixelTransformation<TPixelType> TTransformation>
auto createPixelTransformer(IImageTransformer<TPixelType> &wrapped, TTransformation transformation) noexcept
    -> PixelTransformer<TPixelType, TTransformation>
{
    return PixelTransformer<TPixelType, TTransformation>{wrapped, transformation};
}

/// @brief Helper method to ease the creation of PixelTransformer.
/// This will call the default constructor.
///
/// @tparam TPixelType The pixel type used by the transformer.
/// @tparam TTransformation The type of transformation.
/// @return The created transformer.
template <typename TPixelType, PixelTransformation<TPixelType> TTransformation>
auto createPixelTransformer(IImageTransformer<TPixelType> &wrapped) noexcept
    -> PixelTransformer<TPixelType, TTransformation>
{
    return PixelTransformer<TPixelType, TTransformation>{wrapped, TTransformation{}};
}

} // namespace Terrahertz

#endif // !THZ_IMAGE_TRANSFORMATION_PIXELTRANSFORMER_HPP
