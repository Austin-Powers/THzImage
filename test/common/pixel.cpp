#include "THzImage/common/pixel.h"

#include "THzImage/common/colorspaceconverter.h"

#include <cmath>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct Common_Pixel : public testing::Test
{
    double epsilon{1e-6};
};

TEST_F(Common_Pixel, BGRADefaultConstruction)
{
    BGRAPixel bgraDefault{};
    EXPECT_EQ(bgraDefault.blue, std::uint8_t{});
    EXPECT_EQ(bgraDefault.green, std::uint8_t{});
    EXPECT_EQ(bgraDefault.red, std::uint8_t{});
    EXPECT_EQ(bgraDefault.alpha, 0xFFU);
}

TEST_F(Common_Pixel, BGRAConstruction)
{
    BGRAPixel bgr{1U, 2U, 3U};
    EXPECT_EQ(bgr.blue, 1U);
    EXPECT_EQ(bgr.green, 2U);
    EXPECT_EQ(bgr.red, 3U);
    EXPECT_EQ(bgr.alpha, 0xFFU);

    BGRAPixel bgra{2U, 3U, 4U, 5U};
    EXPECT_EQ(bgra.blue, 2U);
    EXPECT_EQ(bgra.green, 3U);
    EXPECT_EQ(bgra.red, 4U);
    EXPECT_EQ(bgra.alpha, 5U);
}

TEST_F(Common_Pixel, HSVADefaultConstruction)
{
    HSVAPixel hsvaDefault{};
    EXPECT_EQ(hsvaDefault.hue, float{});
    EXPECT_EQ(hsvaDefault.saturation, std::uint8_t{});
    EXPECT_EQ(hsvaDefault.value, std::uint8_t{});
    EXPECT_EQ(hsvaDefault.alpha, 0xFFU);
}

TEST_F(Common_Pixel, HSVAConstruction)
{
    HSVAPixel hsv{1.4F, 2U, 3U};
    EXPECT_EQ(hsv.hue, 1.4F);
    EXPECT_EQ(hsv.saturation, 2U);
    EXPECT_EQ(hsv.value, 3U);
    EXPECT_EQ(hsv.alpha, 0xFFU);

    HSVAPixel hsva{2.4F, 3U, 4U, 5U};
    EXPECT_EQ(hsva.hue, 2.4F);
    EXPECT_EQ(hsva.saturation, 3U);
    EXPECT_EQ(hsva.value, 4U);
    EXPECT_EQ(hsva.alpha, 5U);
}

TEST_F(Common_Pixel, BGRAFromHSVA)
{
    HSVAPixel    hsva{3.4F, 100U, 128U};
    std::uint8_t expectedBlue{};
    std::uint8_t expectedGreen{};
    std::uint8_t expectedRed{};
    HSVtoBGR(hsva.hue, hsva.saturation, hsva.value, expectedBlue, expectedGreen, expectedRed);
    BGRAPixel bgra{hsva};
    EXPECT_EQ(expectedBlue, bgra.blue);
    EXPECT_EQ(expectedGreen, bgra.green);
    EXPECT_EQ(expectedRed, bgra.red);
    EXPECT_EQ(hsva.alpha, bgra.alpha);

    BGRAPixel bgra2{};
    bgra2 = hsva;
    EXPECT_EQ(expectedBlue, bgra2.blue);
    EXPECT_EQ(expectedGreen, bgra2.green);
    EXPECT_EQ(expectedRed, bgra2.red);
    EXPECT_EQ(hsva.alpha, bgra2.alpha);
}

TEST_F(Common_Pixel, HSVAFromBGRA)
{
    BGRAPixel    bgra{123U, 34U, 111U};
    float        expectedHue{};
    std::uint8_t expectedSaturation{};
    std::uint8_t expectedValue{};
    BGRtoHSV(bgra.blue, bgra.green, bgra.red, expectedHue, expectedSaturation, expectedValue);
    HSVAPixel hsva{bgra};
    EXPECT_NEAR(expectedValue, hsva.value, epsilon);
    EXPECT_EQ(expectedSaturation, hsva.saturation);
    EXPECT_EQ(expectedValue, hsva.value);
    EXPECT_EQ(bgra.alpha, hsva.alpha);

    HSVAPixel hsva2{};
    hsva2 = bgra;
    EXPECT_NEAR(expectedValue, hsva2.value, epsilon);
    EXPECT_EQ(expectedSaturation, hsva2.saturation);
    EXPECT_EQ(expectedValue, hsva2.value);
    EXPECT_EQ(bgra.alpha, hsva2.alpha);
}

TEST_F(Common_Pixel, BGRAComparison)
{
    BGRAPixel base{12U, 14U, 56U, 128U};
    EXPECT_EQ(base, base);
    BGRAPixel diff{};
    for (auto i = 0U; i < 15U; ++i)
    {
        diff.blue  = (i & 0x1U) ? base.blue : 123U;
        diff.green = (i & 0x2U) ? base.green : 123U;
        diff.red   = (i & 0x4U) ? base.red : 123U;
        diff.alpha = (i & 0x8U) ? base.alpha : 123U;
        EXPECT_NE(base, diff);
    }
}

TEST_F(Common_Pixel, HSVAComparison)
{
    HSVAPixel base{2.31F, 14U, 56U, 128U};
    EXPECT_EQ(base, base);
    HSVAPixel diff{};
    for (auto i = 0U; i < 15U; ++i)
    {
        diff.hue        = (i & 0x1U) ? base.hue : 1.23F;
        diff.saturation = (i & 0x2U) ? base.saturation : 123U;
        diff.value      = (i & 0x4U) ? base.value : 123U;
        diff.alpha      = (i & 0x8U) ? base.alpha : 123U;
        EXPECT_NE(base, diff);
    }
}

TEST_F(Common_Pixel, BGRADistanceSquared)
{
    BGRAPixel pivot{12U, 27U, 56U};
    EXPECT_EQ(pivot.distanceSquared(pivot), 0U);
    for (auto i = 0; i < 200; i += 42)
    {
        auto const bDist = i - pivot.blue;
        auto const gDist = i - pivot.green;
        auto const rDist = i - pivot.red;

        auto const expectedDistanceSquared = static_cast<size_t>(bDist * bDist + gDist * gDist + rDist * rDist);
        BGRAPixel  target{static_cast<std::uint8_t>(i), static_cast<std::uint8_t>(i), static_cast<std::uint8_t>(i)};
        EXPECT_EQ(pivot.distanceSquared(target), expectedDistanceSquared);
        EXPECT_EQ(target.distanceSquared(pivot), expectedDistanceSquared);
    }
}

TEST_F(Common_Pixel, BGRASubtraction)
{
    BGRAPixel base{40U, 32U, 45U, 192U};
    for (auto i = 0U; i < 240U; i += 42U)
    {
        BGRAPixel toSubtract{static_cast<std::uint8_t>(i),
                             static_cast<std::uint8_t>(i),
                             static_cast<std::uint8_t>(i),
                             static_cast<std::uint8_t>(i)};
        BGRAPixel expectation{0x80U, 0x80U, 0x80U, base.alpha};
        expectation.blue += base.blue - toSubtract.blue;
        expectation.green += base.green - toSubtract.green;
        expectation.red += base.red - toSubtract.red;
        EXPECT_EQ(base - toSubtract, expectation);
    }
}

TEST_F(Common_Pixel, BGRAAddition)
{
    BGRAPixel base{40U, 32U, 45U, 192U};
    for (auto i = 0U; i < 240U; i += 42U)
    {
        BGRAPixel diff{static_cast<std::uint8_t>(i),
                       static_cast<std::uint8_t>(i),
                       static_cast<std::uint8_t>(i),
                       static_cast<std::uint8_t>(i)};

        auto const subtracted = base - diff;
        auto const readded    = subtracted + diff;
        EXPECT_EQ(base, readded);
    }
}

TEST_F(Common_Pixel, BGRALerp)
{
    BGRAPixel a{23U, 34U, 55U, 192U};
    BGRAPixel b{240U, 134U, 0U, 64U};
    EXPECT_EQ(a, lerp(a, b, 0.0F));
    EXPECT_EQ(b, lerp(a, b, 1.0F));
    for (auto t = 0.0F; t < 1.0F; t += 0.008F)
    {
        auto const interpolated = lerp(a, b, t);
        auto       expectation  = a;
        expectation.blue += static_cast<std::uint8_t>(t * (b.blue - a.blue));
        expectation.green += static_cast<std::uint8_t>(t * (b.green - a.green));
        expectation.red += static_cast<std::uint8_t>(t * (b.red - a.red));
        expectation.alpha += static_cast<std::uint8_t>(t * (b.alpha - a.alpha));
        EXPECT_EQ(interpolated, expectation);
    }
}

TEST_F(Common_Pixel, HSVALerp)
{
    auto const compareHSVA = [&](HSVAPixel const &expectation, HSVAPixel const &reality) {
        EXPECT_NEAR(expectation.hue, reality.hue, epsilon);
        EXPECT_EQ(expectation.saturation, reality.saturation);
        EXPECT_EQ(expectation.value, reality.value);
        EXPECT_EQ(expectation.alpha, reality.alpha);
    };

    HSVAPixel a{-0.14F, 34U, 55U, 192U};
    HSVAPixel b{0.23F, 134U, 0U, 64U};
    compareHSVA(a, lerp(a, b, 0.0F));
    compareHSVA(b, lerp(a, b, 1.0F));
    for (auto t = 0.0F; t < 1.0F; t += 0.008F)
    {
        auto const interpolated = lerp(a, b, t);
        auto       expectation  = a;

        auto const ax   = std::cosf(a.hue);
        auto const ay   = std::sinf(a.hue);
        auto const bx   = std::cosf(b.hue);
        auto const by   = std::sinf(b.hue);
        auto const rx   = ax + (t * (bx - ax));
        auto const ry   = ay + (t * (by - ay));
        expectation.hue = atan2f(ry, rx);
        expectation.saturation += static_cast<std::uint8_t>(t * (b.saturation - a.saturation));
        expectation.value += static_cast<std::uint8_t>(t * (b.value - a.value));
        expectation.alpha += static_cast<std::uint8_t>(t * (b.alpha - a.alpha));
        EXPECT_EQ(interpolated, expectation);
    }
}

} // namespace Terrahertz::UnitTests
