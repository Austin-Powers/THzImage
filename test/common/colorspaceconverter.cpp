#include "THzImage/common/colorspaceconverter.hpp"

#include "THzCommon/math/constants.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct CommonColorSpaceConverter : public testing::Test
{};

TEST_F(CommonColorSpaceConverter, BGRtoHSVConversion)
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

TEST_F(CommonColorSpaceConverter, BGRtoHSVtoBGRConversion)
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

TEST_F(CommonColorSpaceConverter, MiniHSVtoBGRConversion)
{
    for (auto h = 0U; h < 8U; ++h)
    {
        auto const hue = static_cast<float>((22.5 + (45.0 * h)) * DegreeToRadian);
        for (auto s = 0U; s < 4U; ++s)
        {
            auto const saturation = 32U + (64U * s);
            for (auto v = 0U; v < 8U; ++v)
            {
                auto const   value = 16U + (32U * v);
                std::uint8_t expectedBlue{};
                std::uint8_t expectedGreen{};
                std::uint8_t expectedRed{};
                HSVtoBGR(hue, saturation, value, expectedBlue, expectedGreen, expectedRed);

                auto const   content = (h << 5U) | (s << 3U) | v;
                std::uint8_t actualBlue{};
                std::uint8_t actualGreen{};
                std::uint8_t actualRed{};
                MiniHSVtoBGR(content, actualBlue, actualGreen, actualRed);
                EXPECT_EQ(expectedBlue, actualBlue);
                EXPECT_EQ(expectedGreen, actualGreen);
                EXPECT_EQ(expectedRed, actualRed);
            }
        }
    }
}

TEST_F(CommonColorSpaceConverter, BGRtoGrayConversion)
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
