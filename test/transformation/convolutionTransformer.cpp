#include "THzImage/transformation/convolutionTransformer.hpp"

#include "THzImage/common/pixel.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct Transformation_ConvolutionTransformer : public testing::Test
{};

TEST_F(Transformation_ConvolutionTransformer, ParameterConstructionThrowIfGivenInvalidValues)
{
    BGRAPixel const pixel{};
    EXPECT_THROW(ConvolutionParameters<BGRAPixel> wrongSizeX(0U, 1U, 1U, 1U, false, pixel), std::invalid_argument);
    EXPECT_THROW(ConvolutionParameters<BGRAPixel> wrongSizeX(1U, 0U, 1U, 1U, false, pixel), std::invalid_argument);
    EXPECT_THROW(ConvolutionParameters<BGRAPixel> wrongSizeX(1U, 1U, 0U, 1U, false, pixel), std::invalid_argument);
    EXPECT_THROW(ConvolutionParameters<BGRAPixel> wrongSizeX(1U, 1U, 1U, 0U, false, pixel), std::invalid_argument);
}

TEST_F(Transformation_ConvolutionTransformer, ParametersReturnCorrectValues)
{
    std::uint16_t const sizeX{7U};
    std::uint16_t const sizeY{5U};
    std::uint16_t const shiftX{2U};
    std::uint16_t const shiftY{1U};
    bool const          border{true};
    BGRAPixel const     borderFill{0x12U, 0x53U, 0x25U, 0x02U};

    ConvolutionParameters const parameters{sizeX, sizeY, shiftX, shiftY, border, borderFill};
    EXPECT_EQ(parameters.sizeX(), sizeX);
    EXPECT_EQ(parameters.sizeY(), sizeY);
    EXPECT_EQ(parameters.shiftX(), shiftX);
    EXPECT_EQ(parameters.shiftY(), shiftY);
    EXPECT_EQ(parameters.border(), border);
    EXPECT_EQ(parameters.borderFill(), borderFill);
}

} // namespace Terrahertz::UnitTests
