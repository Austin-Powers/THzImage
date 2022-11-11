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
    pixel.blue = 0x24U;
    checkCode(pixel);
    pixel.red = 0xFFU;
    checkCode(pixel);
}

// TEST_F(IO_QOIWriter, OpRunHasHighestPriority) { EXPECT_TRUE(compressor.nextPixel(startColor).empty()); }

} // namespace Terrahertz::UnitTests
