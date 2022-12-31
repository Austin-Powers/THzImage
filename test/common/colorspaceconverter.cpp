#include "THzImage/common/colorspaceconverter.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct Common_ColorSpaceConverter : public testing::Test
{};

TEST_F(Common_ColorSpaceConverter, BGRtoHSVConversion)
{
    auto const checkConversion = [](std::uint8_t const b,
                                    std::uint8_t const g,
                                    std::uint8_t const r,
                                    float const        expectedH,
                                    std::uint8_t const expectedS,
                                    std::uint8_t const expectedV) noexcept {
        float        h{};
        std::uint8_t s{};
        std::uint8_t v{};
        BGRtoHSV(b, g, r, h, s, v);
        EXPECT_NEAR(h, expectedH, 1e-9);
        EXPECT_EQ(s, expectedS);
        EXPECT_EQ(v, expectedV);
    };
}

TEST_F(Common_ColorSpaceConverter, BGRtoHSVtoBGRConversion)
{
    auto const checkConversion = [](std::uint8_t const b, std::uint8_t const g, std::uint8_t const r) noexcept {
        float        h{};
        std::uint8_t s{};
        std::uint8_t v{};
        BGRtoHSV(b, g, r, h, s, v);
        std::uint8_t b0{};
        std::uint8_t g0{};
        std::uint8_t r0{};
        HSVtoBGR(h, s, v, b0, g0, r0);
        EXPECT_EQ(b, b0);
        EXPECT_EQ(g, g0);
        EXPECT_EQ(r, r0);
    };

    for (auto r = 0U; r < 256U; r += 5)
    {
        for (auto g = 0U; g < 256U; g += 5)
        {
            for (auto b = 0U; b < 256U; b += 5)
            {
                checkConversion(b, g, r);
            }
        }
    }
}

TEST_F(Common_ColorSpaceConverter, BGRtoGrayConversion)
{
    auto const checkConversion = [](std::uint8_t const b, std::uint8_t const g, std::uint8_t const r) noexcept {
        auto const expectedValue = static_cast<std::uint8_t>((0.0722F * b) + (0.7152F * g) + (0.2126F * r));
        EXPECT_EQ(BGRtoGray(b, g, r), expectedValue);
    };

    for (auto r = 0U; r < 256U; r += 7)
    {
        for (auto g = 0U; g < 256U; g += 4)
        {
            for (auto b = 0U; b < 256U; b += 11)
            {
                checkConversion(b, g, r);
            }
        }
    }
}

} // namespace Terrahertz::UnitTests
