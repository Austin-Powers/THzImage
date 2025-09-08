#include "THzImage/io/autoFileReader.hpp"

#include "THzCommon/logging/logging.hpp"

namespace Terrahertz::AutoFile {

/// @brief Name provider for the THzImage.IO.AutoFile.Reader class.
struct ReaderProject
{
    static constexpr char const *name() noexcept { return "THzImage.IO.AutoFile.Reader"; }
};

Reader::Reader(std::filesystem::path const path, bool extensionOnly) noexcept { reset(path, extensionOnly); }

Reader::~Reader() noexcept { (deinit()); }

void Reader::reset(std::filesystem::path const path, bool extensionOnly) noexcept {}

bool Reader::imagePresent() const noexcept { return false; }

bool Reader::init() noexcept { return false; }

Rectangle Reader::dimensions() const noexcept { return {}; }

bool Reader::read(gsl::span<BGRAPixel> buffer) noexcept { return false; }

void Reader::deinit() noexcept {}

} // namespace Terrahertz::AutoFile
