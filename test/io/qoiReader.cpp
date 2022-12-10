#include "THzImage/io/qoiReader.h"

#include "THzImage/io/qoiWriter.h"

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

    std::string filepath{"testRead.qoi"};

    void SetUp() noexcept override { decompressor.setOutputBuffer(imageSpan); }
};

TEST_F(IO_QOIReader, EmptyDataBufferGiven) { EXPECT_EQ(decompressor.insertDataChunk(gsl::span<std::uint8_t>{}), 0U); }

TEST_F(IO_QOIReader, OpRGBA)
{
    BGRAPixel expectedColor{0x12U, 0x16U, 0x1AU, 0x4BU};
    dataArray[0U]    = OpRGBA;
    dataArray[1U]    = expectedColor.red;
    dataArray[2U]    = expectedColor.green;
    dataArray[3U]    = expectedColor.blue;
    dataArray[4U]    = expectedColor.alpha;
    auto const bytes = dataSpan.subspan(0U, 5U);
    EXPECT_EQ(decompressor.insertDataChunk(bytes), bytes.size());
    EXPECT_EQ(imageArray[0U], expectedColor);
}

TEST_F(IO_QOIReader, OpRGB)
{
    BGRAPixel expectedColor{0x16U, 0x1AU, 0x4BU};
    dataArray[0U]    = OpRGB;
    dataArray[1U]    = expectedColor.red;
    dataArray[2U]    = expectedColor.green;
    dataArray[3U]    = expectedColor.blue;
    auto const bytes = dataSpan.subspan(0U, 4U);
    EXPECT_EQ(decompressor.insertDataChunk(bytes), bytes.size());
    EXPECT_EQ(imageArray[0U], expectedColor);
}

TEST_F(IO_QOIReader, CodeSplitOverTwoDataBuffers) {}

} // namespace Terrahertz::UnitTests
