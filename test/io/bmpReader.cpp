#include "THzImage/io/bmpReader.h"

#include "THzImage/common/image.h"
#include "THzImage/io/bmpWriter.h"

#include <array>
#include <fstream>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_BMPReader : public testing::Test
{
    std::string filepath{"test.bmp"};

    std::array<std::uint8_t, 70U> testFilecontent{
        0x42U, 0x4DU, 0x46U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x36U, 0x00U, 0x00U, 0x00U,
        0x28U, 0x00U, 0x00U, 0x00U, 0x02U, 0x00U, 0x00U, 0x00U, 0x02U, 0x00U, 0x00U, 0x00U, 0x01U, 0x00U,
        0x20U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x10U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
        0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x01U, 0x20U,
        0x25U, 0x24U, 0x13U, 0x24U, 0x50U, 0x34U, 0x31U, 0x22U, 0x05U, 0x20U, 0x01U, 0x44U, 0xFFU, 0x17U};

    void prepareTestFile(gsl::span<std::uint8_t const> buffer) noexcept
    {
        std::ofstream stream{filepath, std::ios::binary};
        ASSERT_TRUE(stream.is_open());
        stream.write(std::bit_cast<char const *>(buffer.data()), buffer.size());
        stream.close();
    }

    void prepareTestFile() noexcept { prepareTestFile(testFilecontent); }
};

TEST_F(IO_BMPReader, ConstructionCorrect)
{
    BMP::Reader sut{filepath};
    EXPECT_FALSE(sut.multipleImages());
    EXPECT_EQ(sut.dimensions(), Rectangle{});
}

TEST_F(IO_BMPReader, NonExistingFile)
{
    BMP::Reader sut{"notHere.bmp"};
    EXPECT_FALSE(sut.init());
}

TEST_F(IO_BMPReader, FileTooSmallForHeader)
{
    std::array<std::uint8_t, 32U> data{};
    prepareTestFile(data);

    BMP::Reader sut{filepath};
    EXPECT_FALSE(sut.init());
}

TEST_F(IO_BMPReader, MagicBytesIncorrect)
{
    testFilecontent[0U] = 0x33U;
    prepareTestFile();

    BMP::Reader sut{filepath};
    EXPECT_FALSE(sut.init());
}

TEST_F(IO_BMPReader, BitCountIncorrect)
{
    testFilecontent[28U] = 0x8U;
    prepareTestFile();

    BMP::Reader sut{filepath};
    EXPECT_FALSE(sut.init());
}

TEST_F(IO_BMPReader, UnsupportedCompressionValue)
{
    testFilecontent[30U] = 0x8U;
    prepareTestFile();

    BMP::Reader sut{filepath};
    EXPECT_FALSE(sut.init());
}

TEST_F(IO_BMPReader, WidthZero)
{
    testFilecontent[18U] = 0x0U;
    prepareTestFile();

    BMP::Reader sut{filepath};
    EXPECT_FALSE(sut.init());
}

TEST_F(IO_BMPReader, HeightZero)
{
    testFilecontent[22U] = 0x0U;
    prepareTestFile();

    BMP::Reader sut{filepath};
    EXPECT_FALSE(sut.init());
}

TEST_F(IO_BMPReader, DimensionsDoNotMatchTheAmountOfData)
{
    testFilecontent[34U] = 0xFFU;
    prepareTestFile();

    BMP::Reader sut{filepath};
    EXPECT_FALSE(sut.init());
}

TEST_F(IO_BMPReader, BufferTooSmallForData)
{
    prepareTestFile();

    BMP::Reader sut{filepath};
    ASSERT_TRUE(sut.init());
    Rectangle const expectedDimensions{2U, 2U};
    ASSERT_EQ(sut.dimensions(), expectedDimensions);
    std::array<BGRAPixel, 2U> arr{};
    EXPECT_FALSE(sut.read(toSpan<BGRAPixel>(arr)));
}

TEST_F(IO_BMPReader, ReadingDataWithTransparency)
{
    BGRAImage expected{};
    ASSERT_TRUE(expected.setDimensions(Rectangle{2U, 2U}));
    expected[0U] = BGRAPixel{1U, 2U, 3U, 4U};
    expected[1U] = BGRAPixel{5U, 6U, 7U, 8U};
    expected[2U] = BGRAPixel{9U, 10U, 11U, 12U};
    expected[3U] = BGRAPixel{13U, 14U, 15U, 16U};

    BMP::Writer writer{filepath};
    ASSERT_TRUE(expected.write(&writer));

    BGRAImage   actual{};
    BMP::Reader reader{filepath};
    ASSERT_TRUE(actual.read(&reader));
    ASSERT_EQ(expected.dimensions(), actual.dimensions());
    for (auto i = 0U; i < 4U; ++i)
    {
        EXPECT_EQ(expected[i], actual[i]);
    }
}

TEST_F(IO_BMPReader, ReadingDataWithoutTransparency)
{
    BGRAImage expected{};
    ASSERT_TRUE(expected.setDimensions(Rectangle{4U, 2U}));
    expected[0U] = BGRAPixel{1U, 2U, 3U};
    expected[1U] = BGRAPixel{5U, 6U, 7U};
    expected[2U] = BGRAPixel{9U, 10U, 11U};
    expected[3U] = BGRAPixel{13U, 14U, 15U};
    expected[4U] = BGRAPixel{17U, 18U, 19U};
    expected[5U] = BGRAPixel{21U, 22U, 23U};
    expected[6U] = BGRAPixel{25U, 26U, 27U};
    expected[7U] = BGRAPixel{29U, 30U, 31U};

    BMP::Writer writer{filepath, false};
    ASSERT_TRUE(expected.write(&writer));

    BGRAImage   actual{};
    BMP::Reader reader{filepath};
    ASSERT_TRUE(actual.read(&reader));
    ASSERT_EQ(expected.dimensions(), actual.dimensions());
    for (auto i = 0U; i < 8U; ++i)
    {
        EXPECT_EQ(expected[i], actual[i]);
    }
}

TEST_F(IO_BMPReader, ReadingDataWithoutTransparencyAndPadding)
{
    BGRAImage expected{};
    ASSERT_TRUE(expected.setDimensions(Rectangle{2U, 2U}));
    expected[0U] = BGRAPixel{1U, 2U, 3U};
    expected[1U] = BGRAPixel{5U, 6U, 7U};
    expected[2U] = BGRAPixel{9U, 10U, 11U};
    expected[3U] = BGRAPixel{13U, 14U, 15U};

    BMP::Writer writer{filepath, false};
    ASSERT_TRUE(expected.write(&writer));

    BGRAImage   actual{};
    BMP::Reader reader{filepath};
    ASSERT_TRUE(actual.read(&reader));
    ASSERT_EQ(expected.dimensions(), actual.dimensions());
    for (auto i = 0U; i < 4U; ++i)
    {
        EXPECT_EQ(expected[i], actual[i]);
    }
}

TEST_F(IO_BMPReader, OffBitsDiffersFrom54)
{
    // increase offset
    testFilecontent[10U] += 8U;
    prepareTestFile();

    BGRAImage   image{};
    BMP::Reader sut{filepath};
    EXPECT_FALSE(image.read(&sut));
}

TEST_F(IO_BMPReader, HeightNegative)
{
    // set height to negative value
    (*std::bit_cast<std::int32_t *>(&testFilecontent[22U])) = -2;
    prepareTestFile();

    BGRAImage   image{};
    BMP::Reader sut{filepath};
    EXPECT_TRUE(image.read(&sut));
    ASSERT_EQ(image.dimensions(), (Rectangle{2U, 2U}));
    EXPECT_EQ(image[0U], (BGRAPixel{0x01U, 0x20U, 0x25U, 0x24U}));
    EXPECT_EQ(image[1U], (BGRAPixel{0x13U, 0x24U, 0x50U, 0x34U}));
    EXPECT_EQ(image[2U], (BGRAPixel{0x31U, 0x22U, 0x05U, 0x20U}));
    EXPECT_EQ(image[3U], (BGRAPixel{0x01U, 0x44U, 0xFFU, 0x17U}));
}

TEST_F(IO_BMPReader, FileTooSmall)
{
    prepareTestFile(toSpan<std::uint8_t>(testFilecontent).subspan(0U, 64U));

    BGRAImage   image{};
    BMP::Reader sut{filepath};
    EXPECT_FALSE(image.read(&sut));
}

} // namespace Terrahertz::UnitTests
