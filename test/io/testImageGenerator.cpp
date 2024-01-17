#include "THzImage/io/testImageGenerator.hpp"

#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/image.hpp"
#include "THzImage/io/pngWriter.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IOTestImageGenerator : public testing::Test
{
    Rectangle const dimensions{768U, 512U};
};

TEST_F(IOTestImageGenerator, Construction)
{
    TestImageGenerator sut{dimensions};
    EXPECT_TRUE(sut.imagePresent());
    EXPECT_EQ(sut.dimensions(), dimensions);
}

} // namespace Terrahertz::UnitTests
