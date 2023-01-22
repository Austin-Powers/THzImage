#include "THzImage/io/qoiReader.hpp"

#include "THzCommon/utility/spanhelpers.hpp"
#include "THzImage/io/qoiWriter.hpp"

#include <array>
#include <cstdint>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_QOIReader : public testing::Test
{
    /// @brief Code for index block: 0b00xxxxxx
    static constexpr std::uint8_t OpIndex{0x00U};

    /// @brief Code for diff block:  0b01xxxxxx
    static constexpr std::uint8_t OpDiff{0x40U};

    /// @brief Code for luma block:  0b10xxxxxx
    static constexpr std::uint8_t OpLuma{0x80U};

    /// @brief Code for run block:   0b11xxxxxx
    static constexpr std::uint8_t OpRun{0xC0U};

    /// @brief Code for RGB block:   0x11111110
    static constexpr std::uint8_t OpRGB{0xFEU};

    /// @brief Code for RGBA block:  0x11111111
    static constexpr std::uint8_t OpRGBA{0xFFU};

    /// @brief Mask for the 2 most significant bits: 0x11000000
    static constexpr std::uint8_t Mask2{0xC0};

    /// @brief The expected value of the first byte after a run was inserted.
    static constexpr std::uint8_t ExpectedRunCode{OpRun | 7U};

    /// @brief The start color of the decompression.
    BGRAPixel const startColor{};

    /// @brief The decompressor to test.
    QOI::Internal::Decompressor decompressor{};

    /// @brief The array for the image data.
    std::array<BGRAPixel, 64U> imageArray{};

    /// @brief The span of the image array.
    gsl::span<BGRAPixel> imageSpan{imageArray};

    /// @brief The array for the data to decompress.
    std::array<std::uint8_t, 128U> dataArray{};

    /// @brief The span of the data to decompress.
    gsl::span<std::uint8_t> dataSpan{dataArray};

    /// @brief Replica of the method for hasing color values used by QOI.
    ///
    /// @param color The color to hash.
    /// @return The hash value of the color.
    std::uint8_t hash(BGRAPixel const &color) noexcept
    {
        return (color.red * 3U + color.green * 5U + color.blue * 7U + color.alpha * 11U) & 0b111111U;
    }

    void addRGBAColor(BGRAPixel const &color) noexcept
    {
        dataArray[0U]    = OpRGBA;
        dataArray[1U]    = color.red;
        dataArray[2U]    = color.green;
        dataArray[3U]    = color.blue;
        dataArray[4U]    = color.alpha;
        auto const bytes = dataSpan.subspan(0U, 5U);
        EXPECT_EQ(decompressor.insertDataChunk(bytes), bytes.size());
        EXPECT_EQ(imageArray[0U], color);
    }

    void SetUp() noexcept override { decompressor.setOutputBuffer(imageSpan); }

    std::string filepath{"testRead.qoi"};

    std::array<std::uint8_t, 32U> testFilecontent{0x71U, 0x6FU, 0x69U, 0x66U,  0x00U, 0x00U, 0x00U,  0x02U,
                                                  0x00U, 0x00U, 0x00U, 0x02U,  0x04U, 0x00U, OpRGBA, 0x12U,
                                                  0x23U, 0x34U, 0x45U, OpRGBA, 0x56U, 0x67U, 0x78U,  0xFFU,
                                                  OpRGB, 0x20U, 0x01U, 0x30U,  OpRGB, 0x00U, 0x00U,  0x00U};

    void prepareTestFile(gsl::span<std::uint8_t const> buffer) noexcept
    {
        std::ofstream stream{filepath, std::ios::binary};
        ASSERT_TRUE(stream.is_open());
        stream.write(std::bit_cast<char const *>(buffer.data()), buffer.size());
        stream.close();
    }

    void prepareTestFile() noexcept { prepareTestFile(testFilecontent); }
};

TEST_F(IO_QOIReader, EmptyDataBufferGiven) { EXPECT_EQ(decompressor.insertDataChunk(gsl::span<std::uint8_t>{}), 0U); }

TEST_F(IO_QOIReader, OpRGBA)
{
    BGRAPixel const expectedColor{0x12U, 0x16U, 0x1AU, 0x4BU};
    addRGBAColor(expectedColor);
}

TEST_F(IO_QOIReader, OpRGB)
{
    BGRAPixel const expectedColor{0x16U, 0x1AU, 0x4BU};
    dataArray[0U]    = OpRGB;
    dataArray[1U]    = expectedColor.red;
    dataArray[2U]    = expectedColor.green;
    dataArray[3U]    = expectedColor.blue;
    auto const bytes = dataSpan.subspan(0U, 4U);
    EXPECT_EQ(decompressor.insertDataChunk(bytes), bytes.size());
    EXPECT_EQ(imageArray[0U], expectedColor);
}

TEST_F(IO_QOIReader, OpIndex)
{
    BGRAPixel const expectedColor{0x14U, 0x15U, 0x32U, 0x4BU};
    addRGBAColor(expectedColor);
    dataArray[0U]    = (OpIndex | hash(expectedColor));
    auto const bytes = dataSpan.subspan(0U, 1U);
    EXPECT_EQ(decompressor.insertDataChunk(bytes), bytes.size());
    EXPECT_EQ(imageArray[1U], expectedColor);
}

TEST_F(IO_QOIReader, OpDiff)
{
    BGRAPixel const expectedColor0{0x15U, 0x32U, 0x4BU, 0x14U};
    addRGBAColor(expectedColor0);
    std::int8_t dRed{1};
    std::int8_t dGreen{-1};
    std::int8_t dBlue{0};

    BGRAPixel const expectedColor1{static_cast<std::uint8_t>(expectedColor0.blue + dBlue),
                                   static_cast<std::uint8_t>(expectedColor0.green + dGreen),
                                   static_cast<std::uint8_t>(expectedColor0.red + dRed),
                                   expectedColor0.alpha};
    dataArray[0U]    = OpDiff | ((dRed + 2U) << 4U) | ((dGreen + 2U) << 2U) | (dBlue + 2U);
    auto const bytes = dataSpan.subspan(0U, 1U);
    EXPECT_EQ(decompressor.insertDataChunk(bytes), bytes.size());
    EXPECT_EQ(imageArray[1U], expectedColor1);
}

TEST_F(IO_QOIReader, OpLuma)
{
    BGRAPixel const expectedColor0{0x32U, 0x4BU, 0x14U, 0x15U};
    addRGBAColor(expectedColor0);
    std::int8_t dGreenRed{4};
    std::int8_t dGreen{-24};
    std::int8_t dGreenBlue{-3};

    BGRAPixel const expectedColor1{static_cast<std::uint8_t>(expectedColor0.blue + dGreen + dGreenBlue),
                                   static_cast<std::uint8_t>(expectedColor0.green + dGreen),
                                   static_cast<std::uint8_t>(expectedColor0.red + dGreen + dGreenRed),
                                   expectedColor0.alpha};
    dataArray[0U]    = OpLuma | (dGreen + 32U);
    dataArray[1U]    = ((dGreenRed + 8U) << 4U) | (dGreenBlue + 8U);
    auto const bytes = dataSpan.subspan(0U, 2U);
    EXPECT_EQ(decompressor.insertDataChunk(bytes), bytes.size());
    EXPECT_EQ(imageArray[1U], expectedColor1);
}

TEST_F(IO_QOIReader, OpRun)
{
    BGRAPixel const expectedColor{0x4BU, 0x14U, 0x15U, 0x32U};
    addRGBAColor(expectedColor);
    auto runLength   = 32U;
    dataArray[0U]    = OpRun | (runLength - 1U);
    auto const bytes = dataSpan.subspan(0U, 1U);
    EXPECT_EQ(decompressor.insertDataChunk(bytes), bytes.size());
    for (auto i = 0U; i < runLength; ++i)
    {
        ASSERT_EQ(imageArray[i + 1U], expectedColor);
    }
    EXPECT_EQ(imageArray[runLength + 1U], BGRAPixel{});
}

TEST_F(IO_QOIReader, CodeSplitOverMultipleDataBuffers)
{
    auto const writeByte = [this](std::uint8_t const byte) noexcept {
        dataArray[0U] = byte;
        ASSERT_EQ(decompressor.insertDataChunk(dataSpan.subspan(0U, 1U)), 1U);
    };
    BGRAPixel const expectedRGBA{0x42U, 0x08U, 0x15U, 0xABU};
    writeByte(OpRGBA);
    writeByte(expectedRGBA.red);
    writeByte(expectedRGBA.green);
    writeByte(expectedRGBA.blue);
    writeByte(expectedRGBA.alpha);
    EXPECT_EQ(imageArray[0U], expectedRGBA);
    BGRAPixel const expectedRGB{0x08U, 0x15U, 0x42U, 0xABU};
    writeByte(OpRGB);
    writeByte(expectedRGB.red);
    writeByte(expectedRGB.green);
    writeByte(expectedRGB.blue);
    EXPECT_EQ(imageArray[1U], expectedRGB);

    std::int8_t dGreenRed{4};
    std::int8_t dGreen{-24};
    std::int8_t dGreenBlue{-3};

    BGRAPixel const expectedLuma{static_cast<std::uint8_t>(expectedRGB.blue + dGreen + dGreenBlue),
                                 static_cast<std::uint8_t>(expectedRGB.green + dGreen),
                                 static_cast<std::uint8_t>(expectedRGB.red + dGreen + dGreenRed),
                                 expectedRGB.alpha};
    writeByte(OpLuma | (dGreen + 32U));
    writeByte(((dGreenRed + 8U) << 4U) | (dGreenBlue + 8U));
    EXPECT_EQ(imageArray[2U], expectedLuma);
}

TEST_F(IO_QOIReader, ImageBufferExhausted)
{
    BGRAPixel const expectedColor{0x14U, 0x15U, 0x4BU, 0x32U};
    addRGBAColor(expectedColor);
    auto runLength   = 32U;
    dataArray[0U]    = OpRun | (runLength - 1U);
    auto const bytes = dataSpan.subspan(0U, 1U);
    EXPECT_EQ(decompressor.insertDataChunk(bytes), bytes.size());
    EXPECT_EQ(decompressor.insertDataChunk(bytes), bytes.size());
    EXPECT_EQ(decompressor.insertDataChunk(bytes), 0U);
    EXPECT_EQ(imageArray[imageArray.size() - 1U], expectedColor);
}

TEST_F(IO_QOIReader, ConstructionCorrect)
{
    QOI::Reader sut{filepath};
    EXPECT_TRUE(sut.imagePresent());
    EXPECT_EQ(sut.dimensions(), Rectangle{});
}

TEST_F(IO_QOIReader, NonExistingFile)
{
    QOI::Reader sut{"notThere.qoi"};
    EXPECT_FALSE(sut.fileTypeFits());
    EXPECT_FALSE(sut.init());
    sut.deinit();
}

TEST_F(IO_QOIReader, FileTooSmallForHeader)
{
    std::array<std::uint8_t, 10U> data{};
    prepareTestFile(data);

    QOI::Reader sut{filepath};
    EXPECT_FALSE(sut.fileTypeFits());
    EXPECT_FALSE(sut.init());
}

TEST_F(IO_QOIReader, MagicBytesIncorrect)
{
    testFilecontent[0U] = 0x33U;
    prepareTestFile();

    QOI::Reader sut{filepath};
    EXPECT_FALSE(sut.fileTypeFits());
    EXPECT_FALSE(sut.init());
}

TEST_F(IO_QOIReader, WidthZero)
{
    testFilecontent[7U] = 0x0U;
    prepareTestFile();

    QOI::Reader sut{filepath};
    EXPECT_TRUE(sut.fileTypeFits());
    EXPECT_FALSE(sut.init());
}

TEST_F(IO_QOIReader, HeightZero)
{
    testFilecontent[11U] = 0x0U;
    prepareTestFile();

    QOI::Reader sut{filepath};
    EXPECT_TRUE(sut.fileTypeFits());
    EXPECT_FALSE(sut.init());
}

TEST_F(IO_QOIReader, BufferTooSmallForData)
{
    prepareTestFile();

    QOI::Reader sut{filepath};
    EXPECT_TRUE(sut.fileTypeFits());
    ASSERT_TRUE(sut.init());
    Rectangle const expectedDimensions{2U, 2U};
    ASSERT_EQ(sut.dimensions(), expectedDimensions);
    std::array<BGRAPixel, 2U> arr{};
    EXPECT_FALSE(sut.read(toSpan<BGRAPixel>(arr)));
}

TEST_F(IO_QOIReader, ReadingData)
{
    prepareTestFile();

    QOI::Reader sut{filepath};
    EXPECT_TRUE(sut.fileTypeFits());
    ASSERT_TRUE(sut.init());
    Rectangle const expectedDimensions{2U, 2U};
    ASSERT_EQ(sut.dimensions(), expectedDimensions);
    std::array<BGRAPixel, 4U> arr{};
    EXPECT_TRUE(sut.read(toSpan<BGRAPixel>(arr)));
    sut.deinit();
    EXPECT_FALSE(sut.imagePresent());
}

} // namespace Terrahertz::UnitTests
