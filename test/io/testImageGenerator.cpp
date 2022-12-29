#include "THzImage/io/testImageGenerator.hpp"

#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/image.hpp"
#include "THzImage/io/pngWriter.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_TestImageGenerator : public testing::Test
{
    Rectangle const dimensions{768U, 512U};
};

TEST_F(IO_TestImageGenerator, Construction)
{
    TestImageGenerator sut{dimensions};
    EXPECT_FALSE(sut.multipleImages());
    EXPECT_EQ(sut.dimensions(), dimensions);
}

} // namespace Terrahertz::UnitTests
