#include "THzImage/common/pixel.hpp"

#include "THzImage/common/colorspaceconverter.hpp"

#include <cmath>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct PixelTests : public testing::Test
{
    double epsilon{1e-6};
};

TEST_F(PixelTests, BGRADefaultConstruction)
{
    BGRAPixel bgraDefault{};
    EXPECT_EQ(bgraDefault.blue, std::uint8_t{});
    EXPECT_EQ(bgraDefault.green, std::uint8_t{});
    EXPECT_EQ(bgraDefault.red, std::uint8_t{});
    EXPECT_EQ(bgraDefault.alpha, 0xFFU);
}

TEST_F(PixelTests, BGRAConstruction)
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

TEST_F(PixelTests, HSVADefaultConstruction)
{
    HSVAPixel hsvaDefault{};
    EXPECT_EQ(hsvaDefault.hue, float{});
    EXPECT_EQ(hsvaDefault.saturation, std::uint8_t{});
    EXPECT_EQ(hsvaDefault.value, std::uint8_t{});
    EXPECT_EQ(hsvaDefault.alpha, 0xFFU);
}

TEST_F(PixelTests, HSVAConstruction)
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

TEST_F(PixelTests, BGRAFromHSVA)
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

TEST_F(PixelTests, HSVAFromBGRA)
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

TEST_F(PixelTests, BGRAComparison)
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

TEST_F(PixelTests, HSVAComparison)
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

TEST_F(PixelTests, BGRADistanceSquared)
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

TEST_F(PixelTests, BGRASubtraction)
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

TEST_F(PixelTests, BGRAAddition)
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

TEST_F(PixelTests, BGRALerp)
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

TEST_F(PixelTests, HSVALerp)
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

#ifdef __GNUC__
        auto const ax = cosf(a.hue);
        auto const ay = sinf(a.hue);
        auto const bx = cosf(b.hue);
        auto const by = sinf(b.hue);
#else
        auto const ax = std::cosf(a.hue);
        auto const ay = std::sinf(a.hue);
        auto const bx = std::cosf(b.hue);
        auto const by = std::sinf(b.hue);
#endif
        auto const rx   = ax + (t * (bx - ax));
        auto const ry   = ay + (t * (by - ay));
        expectation.hue = atan2f(ry, rx);
        expectation.saturation += static_cast<std::uint8_t>(t * (b.saturation - a.saturation));
        expectation.value += static_cast<std::uint8_t>(t * (b.value - a.value));
        expectation.alpha += static_cast<std::uint8_t>(t * (b.alpha - a.alpha));
        EXPECT_EQ(interpolated, expectation);
    }
}

TEST_F(PixelTests, BGRAPixelFloatConstruction)
{
    BGRAPixelFloat sut{};
    EXPECT_EQ(sut.blue, 0.0F);
    EXPECT_EQ(sut.green, 0.0F);
    EXPECT_EQ(sut.red, 0.0F);
    EXPECT_EQ(sut.alpha, 255.0F);
}

TEST_F(PixelTests, BGRAPixelFloatConversion)
{
    BGRAPixel      base{14U, 56U, 21U};
    BGRAPixelFloat sut{base};
    EXPECT_EQ(sut.blue, static_cast<float>(base.blue));
    EXPECT_EQ(sut.green, static_cast<float>(base.green));
    EXPECT_EQ(sut.red, static_cast<float>(base.red));
    EXPECT_EQ(sut.alpha, static_cast<float>(base.alpha));

    base.blue  = 22U;
    base.green = 222U;
    base.red   = 2U;
    sut        = base;
    EXPECT_EQ(sut.blue, static_cast<float>(base.blue));
    EXPECT_EQ(sut.green, static_cast<float>(base.green));
    EXPECT_EQ(sut.red, static_cast<float>(base.red));
    EXPECT_EQ(sut.alpha, static_cast<float>(base.alpha));

    auto const checkLambda = [&](BGRAPixel const &pixel) { EXPECT_EQ(pixel, base); };
    checkLambda(sut);

    BGRAPixel base2 = sut;
    EXPECT_EQ(base, base2);
}

TEST_F(PixelTests, BGRAPixelFloatClampingCorrect)
{
    auto const check0 = [](BGRAPixel const &pixel) {
        EXPECT_EQ(pixel.blue, 0U);
        EXPECT_EQ(pixel.green, 0U);
        EXPECT_EQ(pixel.red, 0U);
        EXPECT_EQ(pixel.alpha, 0U);
    };
    auto const check255 = [](BGRAPixel const &pixel) {
        EXPECT_EQ(pixel.blue, 255U);
        EXPECT_EQ(pixel.green, 255U);
        EXPECT_EQ(pixel.red, 255U);
        EXPECT_EQ(pixel.alpha, 255U);
    };

    {
        BGRAPixelFloat sut{-1.0F, -1.0F, -1.0F, -1.0F};
        check0(sut);
    }
    {
        BGRAPixelFloat sut{255.1F, 255.1F, 255.1F, 255.1F};
        check255(sut);
    }
}

TEST_F(PixelTests, BGRAPixelDiffAbsCorrect)
{
    auto const getDiff = [](std::uint8_t const a, std::uint8_t const b) noexcept -> std::uint8_t {
        return a > b ? a - b : b - a;
    };

    BGRAPixel const fixedColor{128U, 100U, 156U, 123U};

    auto const check = [&](std::uint8_t const value) noexcept {
        BGRAPixel const movingColor{value, value, value, value};
        auto const      result = fixedColor.diffAbs(movingColor);
        EXPECT_EQ(result.blue, getDiff(fixedColor.blue, movingColor.blue));
        EXPECT_EQ(result.green, getDiff(fixedColor.green, movingColor.green));
        EXPECT_EQ(result.red, getDiff(fixedColor.red, movingColor.red));
        EXPECT_EQ(result.alpha, getDiff(fixedColor.alpha, movingColor.alpha));
    };

    for (auto i = 0U; i < 256U; ++i)
    {
        check(i);
    }
}

struct PixelArithmeticTests : public PixelTests
{
    BGRAPixel      a{0x24U, 0x54U, 0x3FU};
    BGRAPixel      b{0x42U, 0x5EU, 0xEFU, 0x11U};
    BGRAPixelFloat c{0.123F, 47.11F, 8.15F};
    BGRAPixelFloat d{123.45F, 6.50F, 19.15F, 0.64F};

    BGRAPixelFloat sut{};

    float expectedBlue{sut.blue};
    float expectedGreen{sut.green};
    float expectedRed{sut.red};
    float expectedAlpha{sut.alpha};

    void checkResult() const noexcept
    {
        EXPECT_NEAR(expectedBlue, sut.blue, epsilon);
        EXPECT_NEAR(expectedGreen, sut.green, epsilon);
        EXPECT_NEAR(expectedRed, sut.red, epsilon);
        EXPECT_NEAR(expectedAlpha, sut.alpha, epsilon);
    }
};

TEST_F(PixelArithmeticTests, Adding)
{
    expectedBlue += a.blue;
    expectedGreen += a.green;
    expectedRed += a.red;
    expectedAlpha += a.alpha;
    sut += a;
    checkResult();

    expectedBlue += b.blue;
    expectedGreen += b.green;
    expectedRed += b.red;
    expectedAlpha += b.alpha;
    sut += b;
    checkResult();

    expectedBlue += c.blue;
    expectedGreen += c.green;
    expectedRed += c.red;
    expectedAlpha += c.alpha;
    sut += c;
    checkResult();

    expectedBlue += d.blue;
    expectedGreen += d.green;
    expectedRed += d.red;
    expectedAlpha += d.alpha;
    sut += d;
    checkResult();
}

TEST_F(PixelArithmeticTests, Subtracting)
{
    expectedBlue -= a.blue;
    expectedGreen -= a.green;
    expectedRed -= a.red;
    expectedAlpha -= a.alpha;
    sut -= a;
    checkResult();

    expectedBlue -= b.blue;
    expectedGreen -= b.green;
    expectedRed -= b.red;
    expectedAlpha -= b.alpha;
    sut -= b;
    checkResult();

    expectedBlue -= c.blue;
    expectedGreen -= c.green;
    expectedRed -= c.red;
    expectedAlpha -= c.alpha;
    sut -= c;
    checkResult();

    expectedBlue -= d.blue;
    expectedGreen -= d.green;
    expectedRed -= d.red;
    expectedAlpha -= d.alpha;
    sut -= d;
    checkResult();
}

TEST_F(PixelArithmeticTests, Multiplying)
{
    sut = c;

    expectedBlue  = sut.blue;
    expectedGreen = sut.green;
    expectedRed   = sut.red;
    expectedAlpha = sut.alpha;

    auto factor = 2.23F;
    expectedBlue *= factor;
    expectedGreen *= factor;
    expectedRed *= factor;
    expectedAlpha *= factor;
    sut *= factor;
    checkResult();
    factor = 0.27F;
    expectedBlue *= factor;
    expectedGreen *= factor;
    expectedRed *= factor;
    expectedAlpha *= factor;
    sut *= factor;
    checkResult();
}

TEST_F(PixelArithmeticTests, Dividing)
{
    sut = d;

    expectedBlue  = sut.blue;
    expectedGreen = sut.green;
    expectedRed   = sut.red;
    expectedAlpha = sut.alpha;

    auto divisor = 2.23F;
    expectedBlue /= divisor;
    expectedGreen /= divisor;
    expectedRed /= divisor;
    expectedAlpha /= divisor;
    sut /= divisor;
    checkResult();
    divisor = 0.27F;
    expectedBlue /= divisor;
    expectedGreen /= divisor;
    expectedRed /= divisor;
    expectedAlpha /= divisor;
    sut /= divisor;
    checkResult();
}

} // namespace Terrahertz::UnitTests
