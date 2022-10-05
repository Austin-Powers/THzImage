#include "THzImage/common/image.h"

#include "THzCommon/math/rectangle.h"
#include "THzImage/common/pixel.h"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct Common_Image : public testing::Test
{
    Rectangle testDimensions{16, 24, 16, 12};
    BGRAImage sut{};
};

TEST_F(Common_Image, ImageConstructionCorrect)
{
    Rectangle const expectedDimensions{};
    EXPECT_EQ(sut.dimensions(), expectedDimensions);
}

TEST_F(Common_Image, SetDimensions)
{
    EXPECT_TRUE(sut.setDimensions(testDimensions));
    EXPECT_EQ(sut.dimensions(), testDimensions);
}

TEST_F(Common_Image, IndexAccess)
{
    EXPECT_TRUE(sut.setDimensions(testDimensions));
    BGRAPixel const expectedDefaultColor{};
    for (auto i = 0U; i < sut.dimensions().area(); ++i)
    {
        EXPECT_EQ(sut[i], expectedDefaultColor);
    }

    BGRAPixel const testColor{12U, 16U, 20U, 24U};
    sut[4U] = testColor;
    EXPECT_EQ(sut[4U], testColor);
}

TEST_F(Common_Image, CreateViewOfEntireImage)
{
    EXPECT_TRUE(sut.setDimensions(testDimensions));
    auto const view = sut.view();
    EXPECT_EQ(view.basePointer(), &sut[0U]);
    testDimensions.upperLeftPoint.x = 0U;
    testDimensions.upperLeftPoint.y = 0U;
    EXPECT_EQ(view.imageDimensions(), testDimensions);
    EXPECT_EQ(view.region(), testDimensions);
}

TEST_F(Common_Image, CreateViewOfImageRegion)
{
    EXPECT_TRUE(sut.setDimensions(testDimensions));
    Rectangle const region{2, 2, 4U, 4U};
    auto const      view = sut.view(region);
    EXPECT_EQ(view.basePointer(), &sut[0U]);
    testDimensions.upperLeftPoint.x = 0U;
    testDimensions.upperLeftPoint.y = 0U;
    EXPECT_EQ(view.imageDimensions(), testDimensions);
    EXPECT_EQ(view.region(), region);
}

} // namespace Terrahertz::UnitTests
