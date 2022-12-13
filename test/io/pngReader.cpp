#include "THzImage/io/pngReader.h"

#include "THzImage/common/image.h"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_PNGReader : public testing::Test
{
    std::string filepath{"test.png"};
};

TEST_F(IO_PNGReader, ConstructionCorrect)
{
    // PNG::Reader sut{filepath};
    // EXPECT_FALSE(sut.multipleImages());
    // EXPECT_EQ(sut.dimensions(), Rectangle{});
}

TEST_F(IO_PNGReader, NonExistingFile)
{
    PNG::Reader sut{"notHere.png"};
    EXPECT_FALSE(sut.init());
}

} // namespace Terrahertz::UnitTests
