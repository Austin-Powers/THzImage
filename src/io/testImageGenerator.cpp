#include "THzImage/io/testImageGenerator.hpp"

#include "THzCommon/logging/logging.hpp"

#include <cmath>

namespace Terrahertz {

/// @brief Name provider for the THzImage.IO.TestImageGenerator class.
struct GeneratorProject
{
    static constexpr char const *name() noexcept { return "THzImage.IO.TestImageGenerator"; }
};

TestImageGenerator::TestImageGenerator(Rectangle const &dimensions) noexcept : _dimensions{dimensions} {}

bool TestImageGenerator::imagePresent() const noexcept { return true; }

bool TestImageGenerator::init() noexcept { return true; }

Rectangle TestImageGenerator::dimensions() const noexcept { return _dimensions; }

bool TestImageGenerator::read(gsl::span<BGRAPixel> buffer) noexcept
{
    if (buffer.size() < _dimensions.area())
    {
        logMessage<LogLevel::Error, GeneratorProject>("Given buffer is too small for the data");
        return false;
    }
    for (auto const pos : _dimensions.range())
    {
        HSVAPixel  pixel{};
        auto const halfWidth{0.5F * _dimensions.width};
        auto const halfHeight{0.5F * _dimensions.height};
        auto const x = pos.x - halfWidth;
        auto const y = pos.y - halfHeight;

        auto const length_sq    = (x * x) + (y * y);
        auto const maxLength_sq = (halfWidth * halfWidth) + (halfHeight * halfHeight);
        auto const ratio        = (length_sq / maxLength_sq) * 5.0F;

#ifdef _WIN32
        pixel.hue = std::atan2f(x, y) + PiF;
#else
        pixel.hue = atan2f(x, y) + PiF;
#endif // _WIN32
        pixel.saturation = ratio > 0.98F ? 0xFFU : static_cast<std::uint8_t>((ratio - 1.0F) * 255.0F);
        pixel.value      = ratio <= 1.02F ? 0xFFU : static_cast<std::uint8_t>((1.0F - ratio) * 255.0F);

        buffer[pos] = pixel;
    }
    return true;
}

void TestImageGenerator::deinit() noexcept {}

} // namespace Terrahertz
