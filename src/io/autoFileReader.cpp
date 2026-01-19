#include "THzImage/io/autoFileReader.hpp"

#include "THzCommon/logging/logging.hpp"
#include "THzCommon/utility/stringhelpers.hpp"
#include "THzImage/io/bmpReader.hpp"
#include "THzImage/io/pngReader.hpp"
#include "THzImage/io/qoiReader.hpp"

#include <array>

namespace Terrahertz::AutoFile {

/// @brief Name provider for the THzImage.IO.AutoFile.Reader class.
struct ReaderProject
{
    static constexpr char const *name() noexcept { return "THzImage.IO.AutoFile.Reader"; }
};

/// @brief Enumeration of all supported file formats.
enum class FileFormat
{
    Unknown,
    BMP,
    PNG,
    QOI
};

/// @brief Returns the order in which to check formats of the given filepath.
///
/// @param path The path of the file to return the order for.
/// @return The order in which to check the files.
static std::array<FileFormat, 4U> orderForFile(std::filesystem::path const &path) noexcept
{
    auto const extension = toLower(path.extension().string());
    if (extension == ".png")
    {
        return {FileFormat::PNG, FileFormat::BMP, FileFormat::QOI, FileFormat::Unknown};
    }
    if (extension == ".bmp")
    {
        return {FileFormat::BMP, FileFormat::PNG, FileFormat::QOI, FileFormat::Unknown};
    }
    if (extension == ".qoi")
    {
        return {FileFormat::QOI, FileFormat::PNG, FileFormat::BMP, FileFormat::Unknown};
    }
    return {FileFormat::Unknown, FileFormat::PNG, FileFormat::BMP, FileFormat::QOI};
}

Reader::Reader(std::filesystem::path const path, ExtensionMode mode) noexcept
{
    static_assert(InnerReaderBufferSize >= sizeof(BMP::Reader), "_innerReaderBuffer too small for BMP::Reader.");
    static_assert(InnerReaderBufferSize >= sizeof(PNG::Reader), "_innerReaderBuffer too small for PNG::Reader.");
    static_assert(InnerReaderBufferSize >= sizeof(QOI::Reader), "_innerReaderBuffer too small for QOI::Reader.");
    reset(path, mode);
}

Reader::~Reader() noexcept { deinit(); }

void Reader::reset(std::filesystem::path const path, ExtensionMode mode) noexcept
{
    deinit();
    _path = path;
    _mode = mode;
}

bool Reader::extensionSupported() const noexcept { return orderForFile(_path)[0U] != FileFormat::Unknown; }

bool Reader::imagePresent() const noexcept { return _path != std::filesystem::path{}; }

bool Reader::init() noexcept
{
    if (_innerReader != nullptr)
    {
        // already initialized
        return false;
    }
    if (!imagePresent())
    {
        return false;
    }

    for (auto const format : orderForFile(_path))
    {
        switch (format)
        {
        case FileFormat::BMP:
            _innerReader = new (_innerReaderBuffer.data()) BMP::Reader(_path);
            break;
        case FileFormat::PNG:
            _innerReader = new (_innerReaderBuffer.data()) PNG::Reader(_path);
            break;
        case FileFormat::QOI:
            _innerReader = new (_innerReaderBuffer.data()) QOI::Reader(_path);
            break;
        default: // FileFormat::Unknown
            break;
        }
        if (_innerReader != nullptr)
        {
            if (_innerReader->init())
            {
                return true;
            }
            else
            {
                deinitInnerReader();
            }
        }
        if (_mode == ExtensionMode::strict)
        {
            break;
        }
    }
    deinit();
    return false;
}

Rectangle Reader::dimensions() const noexcept
{
    if (_innerReader != nullptr)
    {
        return _innerReader->dimensions();
    }
    return {};
}

bool Reader::read(gsl::span<BGRAPixel> buffer) noexcept
{
    if (_innerReader != nullptr)
    {
        return _innerReader->read(buffer);
    }
    return false;
}

void Reader::deinit() noexcept
{
    deinitInnerReader();
    // reset path to not load the same file twice
    _path = std::filesystem::path{};
}

void Reader::deinitInnerReader() noexcept
{
    if (_innerReader != nullptr)
    {
        _innerReader->deinit();
        _innerReader->~IImageReader();
        _innerReader = nullptr;
    }
}

} // namespace Terrahertz::AutoFile
