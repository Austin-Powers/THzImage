#ifndef THZ_TEST_IMAGE_TRANSFORMATION_MOCKTRANSFORMER_HPP
#define THZ_TEST_IMAGE_TRANSFORMATION_MOCKTRANSFORMER_HPP

#include "THzImage/common/iImageTransformer.hpp"

#include <algorithm>
#include <cstdint>
#include <vector>

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
        if (countUpPixel)
        {
            ++transformPixel.blue;
            if (transformPixel.blue == 0U)
            {
                ++transformPixel.green;
                if (transformPixel.green == 0U)
                {
                    ++transformPixel.red;
                }
            }
        }
        return transformReturnValue;
    }

    /// @copydoc IImageTransformer::skip
    bool skip() noexcept override
    {
        calls.push_back(CallType::Skip);
        if (countUpPixel)
        {
            ++transformPixel.blue;
            if (transformPixel.blue == 0U)
            {
                ++transformPixel.green;
                if (transformPixel.green == 0U)
                {
                    ++transformPixel.red;
                }
            }
        }
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

    /// @brief If true each call to transform will increase transformPixel.
    bool countUpPixel{};

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

    /// @brief Counts the calls, of a given type, made to the mock transformer.
    ///
    /// @param type The type of call that shall be counted.
    /// @return The number of calls made to certain type of method.
    std::uint32_t countCalls(CallType const type) const noexcept
    {
        return static_cast<std::uint32_t>(std::count_if(
            calls.cbegin(), calls.cend(), [type](CallType const ct) noexcept -> bool { return type == ct; }));
    }
};

} // namespace Terrahertz::UnitTests

#endif // !THZ_TEST_IMAGE_TRANSFORMATION_MOCKTRANSFORMER_HPP
