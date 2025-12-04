#include "THzImage/processing/easyWriter.hpp"

namespace Terrahertz::ImageProcessing {

EasyWriter::EasyWriter(std::filesystem::path const filepath) noexcept
{
    _writer = ImageSeries::Writer<PNG::Writer>::createWriter(filepath.string());
    // maybe extract filename generation from imageserieswriter to replicate names for appending
}

template <>
bool EasyWriter::write(Image<BGRAPixel> const &image) noexcept
{
    if (_writer)
    {
        return _writer->writeContentOf(image);
    }
    return false;
}

} // namespace Terrahertz::ImageProcessing
