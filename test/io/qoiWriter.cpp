#include "THzImage/io/qoiWriter.h"

#include <cstdint>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_QOIWriter : public testing::Test
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

    /// @brief The start color of the compression.
    BGRAPixel const startColor{};

    /// @brief The compressor to test.
    QOI::Internal::Compressor compressor{};

    /// @brief Replica of the method for hasing color values used by QOI.
    ///
    /// @param color The color to hash.
    /// @return The hash value of the color.
    std::uint8_t hash(BGRAPixel const &color) noexcept
    {
        return (color.red * 3U + color.green * 5U + color.blue * 7U + color.alpha * 11U) & 0b111111U;
    }
};

TEST_F(IO_QOIWriter, OpBGRA)
{
    auto const checkCode = [this](BGRAPixel const &pixel) noexcept {
        auto const code = compressor.nextPixel(pixel);
        ASSERT_EQ(code.size(), 5U);
        EXPECT_EQ(code[0U], OpRGBA);
        EXPECT_EQ(code[1U], pixel.red);
        EXPECT_EQ(code[2U], pixel.green);
        EXPECT_EQ(code[3U], pixel.blue);
        EXPECT_EQ(code[4U], pixel.alpha);
    };

    auto pixel  = startColor;
    pixel.blue  = 0x24U;
    pixel.alpha = 0x12U;
    checkCode(pixel);
    pixel.alpha = 0x24U;
    checkCode(pixel);
    pixel.alpha = 0xFFU;
    checkCode(pixel);
}

TEST_F(IO_QOIWriter, OpRGB)
{
    auto const checkCode = [this](BGRAPixel const &pixel) noexcept {
        auto const code = compressor.nextPixel(pixel);
        ASSERT_EQ(code.size(), 4U);
        EXPECT_EQ(code[0U], OpRGB);
        EXPECT_EQ(code[1U], pixel.red);
        EXPECT_EQ(code[2U], pixel.green);
        EXPECT_EQ(code[3U], pixel.blue);
    };

    auto pixel = startColor;
    pixel.blue = 0x24U;
    checkCode(pixel);
    pixel.green = 0x24U;
    checkCode(pixel);
    pixel.red = 0xF0U;
    checkCode(pixel);
}

TEST_F(IO_QOIWriter, OpRunHasHighestPriority) { EXPECT_TRUE(compressor.nextPixel(startColor).empty()); }

TEST_F(IO_QOIWriter, OpIndex)
{
    auto const firstColor  = BGRAPixel{30U, 0U, 0U};
    auto const secondColor = BGRAPixel{30U, 50U, 0U};
    auto const thirdColor  = BGRAPixel{0U, 0U, 70U};
    ASSERT_EQ(hash(firstColor), hash(thirdColor));

    // check basic functionality
    EXPECT_EQ(compressor.nextPixel(firstColor).size(), 4U);
    EXPECT_EQ(compressor.nextPixel(secondColor).size(), 4U);
    auto code = compressor.nextPixel(firstColor);
    ASSERT_EQ(code.size(), 1U);
    EXPECT_EQ(code[0], hash(firstColor));

    // check if codes are overwritten
    EXPECT_EQ(compressor.nextPixel(thirdColor).size(), 4U);
    EXPECT_EQ(compressor.nextPixel(firstColor).size(), 4U);
    EXPECT_EQ(compressor.nextPixel(thirdColor).size(), 4U);

    // reset
    compressor.reset();
    EXPECT_EQ(compressor.nextPixel(firstColor).size(), 4U);
    EXPECT_EQ(compressor.nextPixel(secondColor).size(), 4U);
    code = compressor.nextPixel(firstColor);
    ASSERT_EQ(code.size(), 1U);
    EXPECT_EQ(code[0], hash(firstColor));
}

TEST_F(IO_QOIWriter, OpDiff)
{
    for (auto r = -2; r < 2; ++r)
    {
        for (auto g = -2; g < 2; ++g)
        {
            for (auto b = -2; b < 2; ++b)
            {
                if ((r == 0) && (g == 0) && (b == 0))
                {
                    // this would trigger an OpRun
                    continue;
                }
                auto pixel = startColor;
                pixel.red += r;
                pixel.green += g;
                pixel.blue += b;
                auto const code = compressor.nextPixel(pixel);
                ASSERT_EQ(code.size(), 1U);
                std::uint8_t expectedCode{OpDiff};
                expectedCode |= (r + 2U) << 4U;
                expectedCode |= (g + 2U) << 2U;
                expectedCode |= (b + 2U);
                EXPECT_EQ(code[0U], expectedCode);
                compressor.reset();
            }
        }
    }
}

} // namespace Terrahertz::UnitTests
