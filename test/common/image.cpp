#include "THzImage/common/image.h"

#include "THzCommon/math/rectangle.h"
#include "THzImage/common/pixel.h"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct Common_Image : public testing::Test
{
    using BGRImage = Image<BGRAPixel>;
    using HSVImage = Image<HSVAPixel>;
};

TEST_F(Common_Image, ImageConstructionCorrect)
{
    BGRImage  sut{};
    Rectangle expectedDimensions{};
    EXPECT_EQ(sut.dimensions(), expectedDimensions);
}

TEST_F(Common_Image, SetDimensions)
{
    BGRImage  sut{};
    Rectangle newDimensions{12, 24, 100, 200};
    EXPECT_TRUE(sut.setDimensions(newDimensions));
    EXPECT_EQ(sut.dimensions(), newDimensions);
}

} // namespace Terrahertz::UnitTests
