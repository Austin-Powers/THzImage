#include "THzImage/io/pngWriter.h"

#include "THzImage/common/image.h"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_PNGWriter : public testing::Test
{
    std::string filepath{"test.png"};
};

TEST_F(IO_PNGWriter, ConstructionCorrect)
{
    // PNG::Reader sut{filepath};
    // EXPECT_FALSE(sut.multipleImages());
    // EXPECT_EQ(sut.dimensions(), Rectangle{});
}

} // namespace Terrahertz::UnitTests
