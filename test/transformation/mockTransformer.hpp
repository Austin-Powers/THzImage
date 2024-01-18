#ifndef THZ_TEST_IMAGE_TRANSFORMATION_MOCKTRANSFORMER_HPP
#define THZ_TEST_IMAGE_TRANSFORMATION_MOCKTRANSFORMER_HPP

#include "THzImage/common/iImageTransformer.hpp"

namespace Terrahertz::UnitTests {

/// @brief Enables mocking of a IImageTransformer for testing purposes.
/// This way of mocking offers better customization through inheritance as well as less console spamming than gmock.
///
/// @tparam TPixelType The type of pixel used by the transformer.
template <typename TPixelType>
struct MockTransformer : public IImageTransformer<TPixelType>
{
    /// @copydoc IImageTransformer::dimensions
    Rectangle dimensions() const noexcept override
    {
        calls.push_back(CallType::Dimensions);
        return dimensionsReturnValue;
    }

    /// @copydoc IImageTransformer::transform
    bool transform(TPixelType &pixel) noexcept override
    {
        calls.push_back(CallType::Transform);
        pixel = transformPixel;
        return transformReturnValue;
    }

    /// @copydoc IImageTransformer::skip
    bool skip() noexcept override
    {
        calls.push_back(CallType::Skip);
        return skipReturnValue;
    }

    /// @copydoc IImageTransformer::reset
    bool reset() noexcept override
    {
        calls.push_back(CallType::Reset);
        return resetReturnValue;
    }

    /// @copydoc IImageTransformer::nextImage
    bool nextImage() noexcept override
    {
        calls.push_back(CallType::NextImage);
        return nextImageReturnValue;
    }

    Rectangle dimensionsReturnValue{};

    TPixelType transformPixel{};

    bool transformReturnValue{};

    bool skipReturnValue{};

    bool resetReturnValue{};

    bool nextImageReturnValue{};

    enum class CallType
    {
        Dimensions,
        Transform,
        Skip,
        Reset,
        NextImage
    };

    /// @brief Stores the types of calls made to the transformer.
    std::vector<CallType> mutable calls{};
};

} // namespace Terrahertz::UnitTests

#endif // !THZ_TEST_IMAGE_TRANSFORMATION_MOCKTRANSFORMER_HPP
