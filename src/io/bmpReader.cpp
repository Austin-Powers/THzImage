#include "THzImage/io/bmpReader.h"

namespace Terrahertz {

BMPReader::BMPReader(std::string_view const filepath) noexcept {}

bool BMPReader::multipleImages() const noexcept { return false; }

bool BMPReader::init() noexcept { return false; }

Rectangle BMPReader::dimensions() const noexcept { return {}; }

bool BMPReader::read(gsl::span<BGRAPixel>) noexcept { return false; }

void BMPReader::deinit() noexcept {}

} // namespace Terrahertz
