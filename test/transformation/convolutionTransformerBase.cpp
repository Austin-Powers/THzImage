#include "THzImage/transformation/convolutionTransformerBase.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/common/pixel.hpp"
#include "THzImage/io/testImageGenerator.hpp"
#include "mockTransformer.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct ConvolutionTransformerBaseTests : public testing::Test
{
    class TestClass : public ConvolutionTransformerBase<BGRAPixel>
    {
    public:
        TestClass(IImageTransformer<BGRAPixel> &base) noexcept : ConvolutionTransformerBase{base} {}
    };

    MockTransformer<BGRAPixel> mockTransformer{};
};

TEST_F(ConvolutionTransformerBaseTests, ResetPassedThroughCorrectly) { TestClass sut{mockTransformer}; }

} // namespace Terrahertz::UnitTests
