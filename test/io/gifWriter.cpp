#include "THzImage/io/gifWriter.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_GIFWriter_ColorReduction : public testing::Test
{
    GIF::Internal::ColorReduction sut{};
};

TEST_F(IO_GIFWriter_ColorReduction, InitialStateCorrect)
{
    auto const colorTable = sut.colorTable();
    ASSERT_EQ(colorTable.size(), 2U);
    EXPECT_EQ(colorTable[0U], (BGRAPixel{0xFFU, 0xFFU, 0xFFU}));
    EXPECT_EQ(colorTable[1U], (BGRAPixel{}));
    for (auto i = 0U; i < 256U; ++i)
    {
        BGRAPixel pixel{static_cast<std::uint8_t>(i), static_cast<std::uint8_t>(i), static_cast<std::uint8_t>(i)};
        EXPECT_EQ(sut.convert(pixel), 0U);
    }
}

TEST_F(IO_GIFWriter_ColorReduction, ConversionResultCloseToOriginal)
{
    BGRAImage          image{};
    TestImageGenerator generator{Rectangle{64U, 64U}};
    EXPECT_TRUE(image.read(&generator));
    sut.analyze(gsl::span<BGRAPixel const>{&image[0U], image.dimensions().area()});
    auto const colorTable = sut.colorTable();

    auto accumulatedDeviations = 0.0;
    for (auto const i : image.dimensions().range())
    {
        auto const oColor = image[i];
        auto const rColor = colorTable[sut.convert(oColor)];
        accumulatedDeviations += oColor.distanceSquared(rColor);
    }
    accumulatedDeviations /= image.dimensions().area();
    EXPECT_NEAR(accumulatedDeviations, 268.63, 0.05);
}

} // namespace Terrahertz::UnitTests
