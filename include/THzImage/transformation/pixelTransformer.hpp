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
    /// @param base The base transformer to wrap.
    /// @param transformation The instance encapsulating the transformation algorithm.
    PixelTransformer(IImageTransformer<TPixelType> &base, TTransformation transformation) noexcept
        : _base{base}, _transformation{transformation}
    {}

    /// @copydoc IImageTransformer::dimensions
    Rectangle dimensions() const noexcept override { return _base.dimensions(); }

    /// @copydoc IImageTransformer::transform
    bool transform(TPixelType &pixel) noexcept override
    {
        auto const result = _base.transform(pixel);
        pixel             = _transformation(pixel);
        return result;
    }

    /// @copydoc IImageTransformer::skip
    bool skip() noexcept override { return _base.skip(); }

    /// @copydoc IImageTransformer::reset
    bool reset() noexcept override { return _base.reset(); }

private:
    /// @brief The base transformer to wrap.
    IImageTransformer<TPixelType> &_base;

    /// @brief The instance encapsulating the transformation algorithm.
    TTransformation _transformation;
};

/// @brief
///
/// @tparam TPixelType
/// @tparam TTransformation
/// @param transformation
/// @return
template <typename TPixelType, PixelTransformation<TPixelType> TTransformation>
auto createPixelTransformer(IImageTransformer<TPixelType> &base, TTransformation transformation) noexcept
    -> PixelTransformer<TPixelType, TTransformation>
{
    return PixelTransformer<TPixelType, TTransformation>{base, transformation};
}

/// @brief
///
/// @tparam TPixelType
/// @tparam TTransformation
/// @return
template <typename TPixelType, PixelTransformation<TPixelType> TTransformation>
auto createPixelTransformer(IImageTransformer<TPixelType> &base) noexcept
    -> PixelTransformer<TPixelType, TTransformation>
{
    return PixelTransformer<TPixelType, TTransformation>{base, TTransformation{}};
}

} // namespace Terrahertz

#endif // !THZ_IMAGE_TRANSFORMATION_PIXELTRANSFORMER_HPP
