#include "THzImage/io/bmpWriter.hpp"

#include "THzCommon/utility/fstreamhelpers.hpp"
#include "THzCommon/utility/spanhelpers.hpp"

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
    imageData[0] = BGRAPixel{0x01U, 0x20U, 0x25U, 0x24U};
    imageData[1] = BGRAPixel{0x13U, 0x24U, 0x50U, 0x34U};
    imageData[2] = BGRAPixel{0x31U, 0x22U, 0x05U, 0x20U};
    imageData[3] = BGRAPixel{0x01U, 0x44U, 0xFFU, 0x17U};
    EXPECT_TRUE(sut.write(dimensions, toSpan<BGRAPixel const>(imageData)));
    sut.deinit();

    std::array<std::uint8_t, 70U> const fileContentExpectation{
        0x42U, 0x4DU, 0x46U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x36U, 0x00U, 0x00U, 0x00U,
        0x28U, 0x00U, 0x00U, 0x00U, 0x02U, 0x00U, 0x00U, 0x00U, 0x02U, 0x00U, 0x00U, 0x00U, 0x01U, 0x00U,
        0x20U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x10U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
        0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x31U, 0x22U,
        0x05U, 0x20U, 0x01U, 0x44U, 0xFFU, 0x17U, 0x01U, 0x20U, 0x25U, 0x24U, 0x13U, 0x24U, 0x50U, 0x34U};

    std::ifstream file{filepath, std::ios::binary};
    ASSERT_TRUE(file.is_open());
    std::array<std::uint8_t, 128U> buffer{};
    EXPECT_EQ(readFromStream(file, buffer), fileContentExpectation.size());
    for (auto i = 0U; i < fileContentExpectation.size(); ++i)
    {
        ASSERT_EQ(buffer[i], fileContentExpectation[i]) << "idx: " << i;
    }
}

TEST_F(IO_BMPWriter, WritingWithoutTransparency)
{
    BMP::Writer sut{filepath, false};
    EXPECT_TRUE(sut.init());

    Rectangle const dimensions{4U, 2U};

    std::array<BGRAPixel, 8U> imageData{};
    imageData[0] = BGRAPixel{0x01U, 0x20U, 0x25U, 0x24U};
    imageData[1] = BGRAPixel{0x13U, 0x24U, 0x50U, 0x34U};
    imageData[2] = BGRAPixel{0x31U, 0x22U, 0x05U, 0x20U};
    imageData[3] = BGRAPixel{0x01U, 0x44U, 0xFFU, 0x17U};
    imageData[4] = BGRAPixel{0x10U, 0x24U, 0xAFU, 0x27U};
    imageData[5] = BGRAPixel{0x31U, 0x42U, 0xFAU, 0x37U};
    imageData[6] = BGRAPixel{0x32U, 0x43U, 0xFBU, 0x38U};
    imageData[7] = BGRAPixel{0x33U, 0x44U, 0xFCU, 0x39U};
    EXPECT_TRUE(sut.write(dimensions, toSpan<BGRAPixel const>(imageData)));
    sut.deinit();

    std::array<std::uint8_t, 78U> const fileContentExpectation{
        0x42U, 0x4DU, 0x4EU, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x36U, 0x00U, 0x00U, 0x00U, 0x28U, 0x00U,
        0x00U, 0x00U, 0x04U, 0x00U, 0x00U, 0x00U, 0x02U, 0x00U, 0x00U, 0x00U, 0x01U, 0x00U, 0x18U, 0x00U, 0x00U, 0x00U,
        0x00U, 0x00U, 0x18U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
        0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x10U, 0x24U, 0xAFU, 0x31U, 0x42U, 0xFAU, 0x32U, 0x43U, 0xFBU, 0x33U,
        0x44U, 0xFCU, 0x01U, 0x20U, 0x25U, 0x13U, 0x24U, 0x50U, 0x31U, 0x22U, 0x05U, 0x01U, 0x44U, 0xFFU};

    std::ifstream file{filepath, std::ios::binary};
    ASSERT_TRUE(file.is_open());
    std::array<std::uint8_t, 128U> buffer{};
    EXPECT_EQ(readFromStream(file, buffer), fileContentExpectation.size());
    for (auto i = 0U; i < fileContentExpectation.size(); ++i)
    {
        ASSERT_EQ(buffer[i], fileContentExpectation[i]) << "idx: " << i;
    }
}

TEST_F(IO_BMPWriter, WritingWithoutTransparencyPadding)
{
    BMP::Writer sut{filepath, false};
    EXPECT_TRUE(sut.init());

    Rectangle const dimensions{2U, 2U};

    std::array<BGRAPixel, 4U> imageData{};
    imageData[0] = BGRAPixel{0x01U, 0x20U, 0x25U, 0x24U};
    imageData[1] = BGRAPixel{0x13U, 0x24U, 0x50U, 0x34U};
    imageData[2] = BGRAPixel{0x31U, 0x22U, 0x05U, 0x20U};
    imageData[3] = BGRAPixel{0x01U, 0x44U, 0xFFU, 0x17U};
    EXPECT_TRUE(sut.write(dimensions, toSpan<BGRAPixel const>(imageData)));
    sut.deinit();

    std::array<std::uint8_t, 70U> const fileContentExpectation{
        0x42U, 0x4DU, 0x46U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x36U, 0x00U, 0x00U, 0x00U,
        0x28U, 0x00U, 0x00U, 0x00U, 0x02U, 0x00U, 0x00U, 0x00U, 0x02U, 0x00U, 0x00U, 0x00U, 0x01U, 0x00U,
        0x18U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x10U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
        0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x31U, 0x22U,
        0x05U, 0x01U, 0x44U, 0xFFU, 0x00U, 0x00U, 0x01U, 0x20U, 0x25U, 0x13U, 0x24U, 0x50U, 0x00U, 0x00U};

    std::ifstream file{filepath, std::ios::binary};
    ASSERT_TRUE(file.is_open());
    std::array<std::uint8_t, 128U> buffer{};
    EXPECT_EQ(readFromStream(file, buffer), fileContentExpectation.size());
    for (auto i = 0U; i < fileContentExpectation.size(); ++i)
    {
        ASSERT_EQ(buffer[i], fileContentExpectation[i]) << "idx: " << i;
    }
}

} // namespace Terrahertz::UnitTests
