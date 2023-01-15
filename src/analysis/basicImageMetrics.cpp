#include "THzImage/analysis/basicImageMetrics.hpp"

#include "THzCommon/logging/logging.hpp"

namespace Terrahertz {

/// @brief Name provider for the THzImage.Analysis.BasicImageMetrics class.
struct MetricsProject
{
    static constexpr char const *name() noexcept { return "THzImage.Analysis.BasicImageMetrics"; }
};

BasicImageMetrics::BasicImageMetrics(std::string_view const filepath) noexcept : _filepath{filepath} {}

bool BasicImageMetrics::init() noexcept { return true; }

bool BasicImageMetrics::write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept
{
    if (dimensions.area() != buffer.size())
    {
        logMessage<LogLevel::Error, MetricsProject>("Image dimensions do not match the given buffer size");
        return false;
    }

    // As string_view is not zero terminated, we copy it just to be save when opening the stream.
    std::array<char, 512U> filepath{};
    std::memcpy(filepath.data(), _filepath.data(), std::min(filepath.size(), _filepath.size()));

    std::ofstream stream{filepath.data()};
    if (!stream.is_open())
    {
        logMessage<LogLevel::Error, MetricsProject>("Could not open the file to write");
        return false;
    }

    // TODO implement rest

    // dimensions
    // counters for each channel
    // counters for each value

    return true;
}

void BasicImageMetrics::deinit() noexcept {}

} // namespace Terrahertz
