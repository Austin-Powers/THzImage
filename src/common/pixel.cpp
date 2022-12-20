#include "THzImage/common/pixel.h"

#include "THzCommon/math/constants.hpp"
#include "THzImage/common/colorspaceconverter.h"

#include <algorithm>
#include <cmath>

namespace Terrahertz {

BGRAPixel::BGRAPixel(HSVAPixel const &other) noexcept { *this = other; }

BGRAPixel &BGRAPixel::operator=(HSVAPixel const &other) noexcept
{
    alpha = other.alpha;
    HSVtoBGR(other.hue, other.saturation, other.value, blue, green, red);
    return *this;
}

bool BGRAPixel::operator==(BGRAPixel const &other) const noexcept
{
    return blue == other.blue && green == other.green && red == other.red && alpha == other.alpha;
}

bool BGRAPixel::operator!=(BGRAPixel const &other) const noexcept
{
    return blue != other.blue || green != other.green || red != other.red || alpha != other.alpha;
}

size_t BGRAPixel::distanceSquared(BGRAPixel const &other) const noexcept
{
    auto const bDist = static_cast<int32_t>(blue) - other.blue;
    auto const gDist = static_cast<int32_t>(green) - other.green;
    auto const rDist = static_cast<int32_t>(red) - other.red;
    return static_cast<size_t>(bDist * bDist + gDist * gDist + rDist * rDist);
}

BGRAPixel BGRAPixel::operator+(BGRAPixel const &other) const noexcept
{
    // init the result with neutral grey, to reverse the subtraction
    BGRAPixel result{0x80U, 0x80U, 0x80U, alpha};
    result.red += red + other.red;
    result.green += green + other.green;
    result.blue += blue + other.blue;
    return result;
}

BGRAPixel BGRAPixel::operator-(BGRAPixel const &other) const noexcept
{
    // init the result with neutral grey, as this makes delta images easier to look at
    // and helps compression algorithms as 0 and 255 are now -1 apart from each other
    BGRAPixel result{0x80U, 0x80U, 0x80U, alpha};
    result.red += red - other.red;
    result.green += green - other.green;
    result.blue += blue - other.blue;
    return result;
}

HSVAPixel::HSVAPixel(BGRAPixel const &other) noexcept { *this = other; }

HSVAPixel &HSVAPixel::operator=(BGRAPixel const &other) noexcept
{
    alpha = other.alpha;
    BGRtoHSV(other.blue, other.green, other.red, hue, saturation, value);
    return *this;
}

bool HSVAPixel::operator==(HSVAPixel const &other) const noexcept
{
    return value == other.value && hue == other.hue && saturation == other.saturation && alpha == other.alpha;
}

bool HSVAPixel::operator!=(HSVAPixel const &other) const noexcept
{
    return value != other.value || hue != other.hue || saturation != other.saturation || alpha != other.alpha;
}

BGRAPixel lerp(BGRAPixel const &a, BGRAPixel const &b, float const t) noexcept
{
    BGRAPixel result = a;
    result.blue += static_cast<uint8_t>(t * (b.blue - a.blue));
    result.green += static_cast<uint8_t>(t * (b.green - a.green));
    result.red += static_cast<uint8_t>(t * (b.red - a.red));
    result.alpha += static_cast<uint8_t>(t * (b.alpha - a.alpha));
    return result;
}

HSVAPixel lerp(HSVAPixel const &a, HSVAPixel const &b, float const t) noexcept
{
    HSVAPixel result = a;
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
    auto const rx = ax + (t * (bx - ax));
    auto const ry = ay + (t * (by - ay));
    result.hue    = atan2f(ry, rx);
    result.saturation += static_cast<uint8_t>(t * (b.saturation - a.saturation));
    result.value += static_cast<uint8_t>(t * (b.value - a.value));
    result.alpha += static_cast<uint8_t>(t * (b.alpha - a.alpha));
    return result;
}

} // namespace Terrahertz
