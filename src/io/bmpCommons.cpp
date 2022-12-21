#include "bmpCommons.h"

namespace Terrahertz::BMP {

std::optional<LineSelector> LineSelector::create(gsl::span<BGRAPixel> buffer, std::uint32_t width, bool back) noexcept
{
    if ((buffer.size() % width) == 0U)
    {
        return LineSelector{buffer, width, back};
    }
    return {};
}

gsl::span<BGRAPixel> LineSelector::nextLine() noexcept
{
    gsl::span<BGRAPixel> result{};
    if (!_buffer.empty())
    {
        if (_back)
        {
            result  = _buffer.subspan(_buffer.size() - _width, _width);
            _buffer = _buffer.subspan(0U, _buffer.size() - _width);
        }
        else
        {
            result  = _buffer.subspan(0U, _width);
            _buffer = _buffer.subspan(_width);
        }
    }
    return result;
}

LineSelector::LineSelector(gsl::span<BGRAPixel> buffer, std::uint32_t width, bool back) noexcept
    : _buffer{buffer}, _width{width}, _back{back}
{}

} // namespace Terrahertz::BMP
