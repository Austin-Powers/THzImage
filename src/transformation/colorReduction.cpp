#include "THzImage/transformation/colorReduction.hpp"

namespace Terrahertz {

ColorReduction::ColorReduction(IImageTransformer<BGRAPixel> &base,
                               bool const                    gray,
                               std::uint16_t const           targetCount,
                               bool const                    dithering) noexcept
{}

ColorReduction::ColorReduction(gsl::span<BGRAPixel const> buffer,
                               Rectangle const           &dimensions,
                               bool const                 gray,
                               std::uint16_t const        targetCount,
                               bool const                 dithering) noexcept
{}

Rectangle ColorReduction::dimensions() const noexcept { return {}; }

bool ColorReduction::transform(BGRAPixel &pixel) noexcept { return false; }

bool ColorReduction::skip() noexcept { return false; }

bool ColorReduction::reset() noexcept { return false; }

gsl::span<BGRAPixel const> ColorReduction::colorTable() noexcept { return {}; }

std::optional<std::uint16_t> ColorReduction::transformToIndex() noexcept { return {}; }

} // namespace Terrahertz
