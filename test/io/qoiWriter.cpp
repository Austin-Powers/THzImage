#include "THzImage/io/qoiWriter.h"

#include "THzCommon/utility/fstreamhelpers.hpp"
#include "THzCommon/utility/spanhelpers.hpp"

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

    /// @brief The expected value of the first byte after a run was inserted.
    static constexpr std::uint8_t ExpectedRunCode{OpRun | 7U};

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

    /// @brief Inserts a run into the compressor.
    void insertRun() noexcept
    {
        for (auto i = 0U; i < 8U; ++i)
        {
            EXPECT_EQ(compressor.nextPixel(startColor).size(), 0U);
        }
    }

    std::string filepath{"testWrite.qoi"};
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

TEST_F(IO_QOIWriter, OpLuma)
{
    auto const inDiffRange = [](std::int32_t const value) noexcept -> bool { return -2 <= value && value <= 1; };

    for (auto g = -32; g < 32; ++g)
    {
        for (auto gr = -8; gr < 8; ++gr)
        {
            for (auto gb = -8; gb < 8; ++gb)
            {
                auto const r = gr + g;
                auto const b = gb + g;
                if (inDiffRange(r) && inDiffRange(g) && inDiffRange(b))
                {
                    continue;
                }
                auto pixel = startColor;
                pixel.red += r;
                pixel.green += g;
                pixel.blue += b;
                auto const code = compressor.nextPixel(pixel);
                ASSERT_EQ(code.size(), 2U);
                std::uint8_t const expectedCode0{static_cast<std::uint8_t>(OpLuma | (g + 32U))};
                std::uint8_t const expectedCode1{static_cast<std::uint8_t>((gr + 8U) << 4U | (gb + 8U))};
                ASSERT_EQ(code[0U], expectedCode0);
                ASSERT_EQ(code[1U], expectedCode1);
                compressor.reset();
            }
        }
    }
}

TEST_F(IO_QOIWriter, OpRunOnRunLongerThan61)
{
    auto pixel = startColor;
    for (auto i = 0U; i < 61U; i++)
    {
        ASSERT_EQ(compressor.nextPixel(pixel).size(), 0U);
    }
    auto code = compressor.nextPixel(pixel);
    ASSERT_EQ(code.size(), 1U);
    EXPECT_EQ(code[0U], OpRun | 61U);
    for (auto i = 0U; i < 61U; i++)
    {
        ASSERT_EQ(compressor.nextPixel(pixel).size(), 0U);
    }
    code = compressor.nextPixel(pixel);
    ASSERT_EQ(code.size(), 1U);
    EXPECT_EQ(code[0U], OpRun | 61U);
}

TEST_F(IO_QOIWriter, OpRunFollowedByOpDiff)
{
    insertRun();
    auto color = startColor;
    color.blue += 1U;
    color.green += 1U;
    color.red += 1U;
    auto const code = compressor.nextPixel(color);
    ASSERT_EQ(code.size(), 2U);
    EXPECT_EQ(code[0U], ExpectedRunCode);
    EXPECT_EQ(code[1U], (OpDiff | 0b111111));
}

TEST_F(IO_QOIWriter, OpRunFollowedByOpIndex)
{
    BGRAPixel const otherColor{0x3F, 0x3F, 0x3F};
    EXPECT_EQ(compressor.nextPixel(otherColor).size(), 4U);
    EXPECT_EQ(compressor.nextPixel(startColor).size(), 1U);
    insertRun();
    auto const code = compressor.nextPixel(otherColor);
    ASSERT_EQ(code.size(), 2U);
    EXPECT_EQ(code[0U], ExpectedRunCode);
    EXPECT_EQ(code[1U], (OpIndex | hash(otherColor)));
}

TEST_F(IO_QOIWriter, OpRunFollowedByOpLuma)
{
    insertRun();
    auto color = startColor;
    color.blue += 23U;
    color.green += 31U;
    color.red += 23U;
    auto const code = compressor.nextPixel(color);
    ASSERT_EQ(code.size(), 3U);
    EXPECT_EQ(code[0U], ExpectedRunCode);
    EXPECT_EQ(code[1U], (OpLuma | 0b111111));
    EXPECT_EQ(code[2U], 0U);
}

TEST_F(IO_QOIWriter, OpRunFollowedByOpRGB)
{
    insertRun();
    auto color = startColor;
    color.blue += 60U;
    color.green += 60U;
    color.red += 60U;
    auto const code = compressor.nextPixel(color);
    ASSERT_EQ(code.size(), 5U);
    EXPECT_EQ(code[0U], ExpectedRunCode);
    EXPECT_EQ(code[1U], OpRGB);
    EXPECT_EQ(code[2U], 60U);
    EXPECT_EQ(code[3U], 60U);
    EXPECT_EQ(code[4U], 60U);
}

TEST_F(IO_QOIWriter, OpRunFollowedByOpRGBA)
{
    insertRun();
    auto color      = startColor;
    color.alpha     = 60U;
    auto const code = compressor.nextPixel(color);
    ASSERT_EQ(code.size(), 6U);
    EXPECT_EQ(code[0U], ExpectedRunCode);
    EXPECT_EQ(code[1U], OpRGBA);
    EXPECT_EQ(code[2U], 0U);
    EXPECT_EQ(code[3U], 0U);
    EXPECT_EQ(code[4U], 0U);
    EXPECT_EQ(code[5U], 60U);
}

TEST_F(IO_QOIWriter, FlushReturnsEmptyAfterNotOpRunCode)
{
    auto const checkCase = [this](size_t const      expectedCodeLength,
                                  std::int8_t const dRed,
                                  std::int8_t const dGreen,
                                  std::int8_t const dBlue,
                                  std::int8_t const dAlpha = 0) noexcept {
        auto color = startColor;
        color.red += dRed;
        color.green += dGreen;
        color.blue += dBlue;
        color.alpha += dAlpha;
        ASSERT_EQ(compressor.nextPixel(color).size(), expectedCodeLength);
        EXPECT_EQ(compressor.flush().size(), 0U);
    };
    // OpDiff
    checkCase(1U, 1, 1, 1);
    // OpLuma
    checkCase(2U, 24, 24, 24);
    // OpRGB
    checkCase(4U, 20, 120, 20);
    // OpRGBA
    checkCase(5U, 1, 1, 1, 1);

    // OpIndex
    auto const delta = 123;
    auto       color = startColor;
    color.red += delta;
    color.green += delta;
    color.blue += delta;
    ASSERT_EQ(compressor.nextPixel(color).size(), 4U);
    ASSERT_EQ(compressor.nextPixel(startColor).size(), 1U);
    checkCase(1U, delta, delta, delta);
}

TEST_F(IO_QOIWriter, FlushReturnsOpRunCodeIfPresent)
{
    for (auto i = 0U; i < 8U; ++i)
    {
        ASSERT_EQ(compressor.nextPixel(startColor).size(), 0U);
    }
    auto const code = compressor.flush();
    ASSERT_EQ(code.size(), 1U);
    EXPECT_EQ(code[0U], (OpRun | 7U));
}

TEST_F(IO_QOIWriter, DimensionsDoNotFitBufferSize)
{
    QOI::Writer sut{filepath};
    EXPECT_TRUE(sut.init());
    Rectangle const dimensions{0, 0, 20U, 20U};

    std::array<BGRAPixel, 4U> imageData{};
    EXPECT_FALSE(sut.write(dimensions, toSpan<BGRAPixel const>(imageData)));
    sut.deinit();
}

TEST_F(IO_QOIWriter, Writing)
{
    QOI::Writer sut{filepath};
    EXPECT_TRUE(sut.init());
    Rectangle const dimensions{0, 0, 10U, 10U};

    std::array<BGRAPixel, 100U> imageData{};
    EXPECT_TRUE(sut.write(dimensions, toSpan<BGRAPixel const>(imageData)));
    sut.deinit();

    std::ifstream                       file{filepath, std::ios::binary};
    std::array<std::uint8_t, 128U>      buffer{};
    std::array<std::uint8_t, 16U> const fileContentExpectation{
        0x71U, 0x6FU, 0x69U, 0x66U, 0x00U, 0x00U, 0x00U, 0x0AU, 0x00U, 0x00U, 0x00U, 0x0AU, 0x04U, 0x00U, 0xFDU, 0xE5U};
    ASSERT_EQ(readFromStream(file, buffer), fileContentExpectation.size());
    for (auto i = 0U; i < fileContentExpectation.size(); ++i)
    {
        ASSERT_EQ(buffer[i], fileContentExpectation[i]) << "idx: " << i;
    }
}

} // namespace Terrahertz::UnitTests
