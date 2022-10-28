#include "THzImage/io/qoiWriter.h"

namespace Terrahertz::QOI {
namespace Internal {

Compressor::Compressor() noexcept { reset(); }

void Compressor::reset() noexcept {}

gsl::span<std::uint8_t const> Compressor::nextPixel(BGRAPixel const &pixel) noexcept { return {}; }

} // namespace Internal

Writer::Writer(std::string_view const filepath) noexcept {}

bool Writer::init() noexcept { return true; }

bool Writer::write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept { return false; }

void Writer::deinit() noexcept {}

} // namespace Terrahertz::QOI
