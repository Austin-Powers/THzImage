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

} // namespace Terrahertz::UnitTests
