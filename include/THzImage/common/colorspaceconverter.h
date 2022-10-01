#ifndef THZ_IMAGE_COMMON_COLORSPACECONVERTER_H
#define THZ_IMAGE_COMMON_COLORSPACECONVERTER_H

#include "THzCommon/math/constants.h"

#include <algorithm>
#include <cstdint>

namespace Terrahertz {

/// @brief Converts a BGR to a HSV Color.
///
/// @param blue The blue value of the BGR color.
/// @param green The green value of the BGR color.
/// @param red The red value of the BGR color.
/// @param hue Output: The hue of the HSV color [rad].
/// @param saturation Output: The saturation of the HSV color.
/// @param value Output: The value of the HSV color.
inline void BGRtoHSV(std::uint8_t const blue,
                     std::uint8_t const green,
                     std::uint8_t const red,
                     float             &hue,
                     std::uint8_t      &saturation,
                     std::uint8_t      &value) noexcept
{
    auto max = std::max(std::max(blue, green), red);
    auto min = std::min(std::min(blue, green), red);
    // Hue
    if (min == max)
    {
        hue = 0.0;
    }
    else if (red == max)
    {
        hue = PiF / 3.0f * ((green - blue) / static_cast<float>(max - min));
    }
    else if (green == max)
    {
        hue = PiF / 3.0f * (2 + ((blue - red) / static_cast<float>(max - min)));
    }
    else // if (blue == max)
    {
        hue = PiF / 3.0f * (4 + ((red - green) / static_cast<float>(max - min)));
    }
    if (hue < 0.0f)
    {
        hue += 2.0f * PiF;
    }

    // Saturation
    saturation = ((max == 0) ? 0 : static_cast<std::uint8_t>(((max - min) / static_cast<float>(max)) * 255.0f));

    // Value
    value = max;
}

/// @brief Converts a HSV to a BGR Color.
///
/// @param hue The hue of the HSV color [rad].
/// @param saturation The saturation of the HSV color.
/// @param value The value of the HSV color.
/// @param blue Output: The blue value of the BGR color.
/// @param green Output: The green value of the BGR color.
/// @param red Output: The red value of the BGR color.
inline void HSVtoBGR(float const        hue,
                     std::uint8_t const saturation,
                     std::uint8_t const value,
                     std::uint8_t      &blue,
                     std::uint8_t      &green,
                     std::uint8_t      &red) noexcept
{
    if (value == 0 || saturation == 0)
    {
        blue = green = red = value;
    }
    else
    {
        // accounting for rounding errors
        auto const s  = saturation / 255.01f;
        auto       hi = static_cast<std::uint8_t>(hue / (PiF / 3.0f));
        auto       f  = hue / (PiF / 3.0f) - hi;
        auto       p  = value * (1.0f - s);
        auto       q  = value * (1.0f - s * f);
        auto       t  = value * (1.0f - s * (1.0f - f));
        switch (hi)
        {
        case 0:
        case 6:
            red   = static_cast<std::uint8_t>(value);
            green = static_cast<std::uint8_t>(t);
            blue  = static_cast<std::uint8_t>(p);
            break;
        case 1:
            red   = static_cast<std::uint8_t>(q);
            green = static_cast<std::uint8_t>(value);
            blue  = static_cast<std::uint8_t>(p);
            break;
        case 2:
            red   = static_cast<std::uint8_t>(p);
            green = static_cast<std::uint8_t>(value);
            blue  = static_cast<std::uint8_t>(t);
            break;
        case 3:
            red   = static_cast<std::uint8_t>(p);
            green = static_cast<std::uint8_t>(q);
            blue  = static_cast<std::uint8_t>(value);
            break;
        case 4:
            red   = static_cast<std::uint8_t>(t);
            green = static_cast<std::uint8_t>(p);
            blue  = static_cast<std::uint8_t>(value);
            break;
        case 5:
            red   = static_cast<std::uint8_t>(value);
            green = static_cast<std::uint8_t>(p);
            blue  = static_cast<std::uint8_t>(q);
            break;
        }
    }
}

} // namespace Terrahertz

#endif // !THZ_IMAGE_COMMON_COLORSPACECONVERTER_H
