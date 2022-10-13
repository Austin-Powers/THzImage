#include "THzImage/io/bmpWriter.h"

#include "THzCommon/utility/fstreamhelpers.h"
#include "THzCommon/utility/spanhelpers.h"

#include <array>
#include <fstream>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_BMPWriter : public testing::Test
{
    std::string filepath{"test.bmp"};
};

TEST_F(IO_BMPWriter, DimensionsDoNotFitTheBufferSize)
{
    BMP::Writer sut{filepath, true};
    EXPECT_TRUE(sut.init());
    Rectangle const dimensions{0, 0, 20U, 20U};

    std::array<BGRAPixel, 4U> imageData{};
    EXPECT_FALSE(sut.write(dimensions, toSpan<BGRAPixel const>(imageData)));
    sut.deinit();
}

TEST_F(IO_BMPWriter, WritingWithTransparency)
{
    BMP::Writer sut{filepath, true};
    EXPECT_TRUE(sut.init());

    Rectangle const dimensions{12, 24, 2U, 2U};

    std::array<BGRAPixel, 4U> imageData{};
    imageData[0] = BGRAPixel{123U, 24U, 25U, 240U};
    imageData[1] = BGRAPixel{13U, 224U, 205U, 34U};
    imageData[2] = BGRAPixel{131U, 22U, 5U, 205U};
    imageData[3] = BGRAPixel{1U, 44U, 255U, 170U};
    EXPECT_TRUE(sut.write(dimensions, toSpan<BGRAPixel const>(imageData)));
    sut.deinit();

    std::ifstream file{filepath, std::ios::binary};
    ASSERT_TRUE(file.is_open());
    std::array<std::uint8_t, 128U> buffer{};
    EXPECT_EQ(readFromStream(file, buffer), 70U);
}

TEST_F(IO_BMPWriter, WritingWithoutTransparency) {}

TEST_F(IO_BMPWriter, WritingWithoutTransparencyPadding) {}

} // namespace Terrahertz::UnitTests
