#include "THzImage/io/pngReader.hpp"

#include "THzCommon/logging/logging.hpp"

#include <array>
#include <cstdio>
#include <cstring>
#include <png.h>
#include <vector>

namespace Terrahertz::PNG {

/// @brief Name provider for the THzImage.IO.PNG.Reader class.
struct ReaderProject
{
    static constexpr char const *name() noexcept { return "THzImage.IO.PNG.Reader"; }
};

/// @brief The amount of bytes to check at the beginning of the file to determine if this is a PNG-file.
constexpr uint8_t pngBytesToCheck{4};

/// @brief Implementation of the PNG::Reader.
struct Reader::Impl
{
    Impl(std::string_view const filepath) noexcept
    {
        // As string_view is not zero terminated, we copy it just to be save when opening the stream.
        std::array<char, 512U> path{};
        std::memcpy(path.data(), filepath.data(), std::min(path.size(), filepath.size()));
#ifdef _WIN32
        fopen_s(&_pngFile, path.data(), "rb");
#else
        _pngFile = fopen(path.data(), "rb");
#endif
    }

    ~Impl() noexcept { deinit(); }

    bool fileTypeFits() noexcept
    {
        if (_pngFile == nullptr)
        {
            return false;
        }
        // check if png
        png_byte buf[pngBytesToCheck];
        if (fread(buf, 1, pngBytesToCheck, _pngFile) != pngBytesToCheck)
        {
            return false;
        }
        fseek(_pngFile, 0, SEEK_SET);
        if (png_sig_cmp(buf, (png_size_t)0, pngBytesToCheck))
        {
            return false;
        }
        return true;
    }

    bool imagePresent() const noexcept
    {
        // stream is opened at construction and will be closed on deinit
        // creating the desired outputs
        return _pngFile != nullptr;
    }

    bool init() noexcept
    {
        if (_pngFile == nullptr)
        {
            logMessage<LogLevel::Error, ReaderProject>("PNG-file could not be opened");
            return false;
        }
        // check if png
        png_byte buf[pngBytesToCheck];
        if (fread(buf, 1, pngBytesToCheck, _pngFile) != pngBytesToCheck)
        {
            logMessage<LogLevel::Error, ReaderProject>("PNG-file header could not be read");
            return false;
        }
        fseek(_pngFile, 0, SEEK_SET);
        if (png_sig_cmp(buf, (png_size_t)0, pngBytesToCheck))
        {
            logMessage<LogLevel::Error, ReaderProject>("PNG-file header was incorrect");
            return false;
        }

        // init PNG-read
        _png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (_png_ptr == nullptr)
        {
            logMessage<LogLevel::Error, ReaderProject>("Unable to create PNG read structure");
            return false;
        }
        _info_ptr = png_create_info_struct(_png_ptr);
        if (_info_ptr == nullptr)
        {
            logMessage<LogLevel::Error, ReaderProject>("Unable to create PNG info structure");
            return false;
        }
        if (setjmp(png_jmpbuf(_png_ptr)))
        {
            logMessage<LogLevel::Error, ReaderProject>("setjmp for PNG reading failed");
            return false;
        }

        png_init_io(_png_ptr, _pngFile);
        png_set_sig_bytes(_png_ptr, 0);
        png_read_info(_png_ptr, _info_ptr);

        png_uint_32 cWidth, cHeight;
        int         bit_depth, color_type, interlace_type;

        png_get_IHDR(
            _png_ptr, _info_ptr, &cWidth, &cHeight, &bit_depth, &color_type, &interlace_type, nullptr, nullptr);
        png_set_strip_16(_png_ptr);
        png_set_packing(_png_ptr);
        if (color_type == PNG_COLOR_TYPE_PALETTE)
        {
            png_set_palette_to_rgb(_png_ptr);
        }
        if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        {
            png_set_expand_gray_1_2_4_to_8(_png_ptr);
        }
        if (png_get_valid(_png_ptr, _info_ptr, PNG_INFO_tRNS) != 0)
        {
            png_set_tRNS_to_alpha(_png_ptr);
        }
        if (png_get_valid(_png_ptr, _info_ptr, PNG_INFO_sBIT) != 0)
        {
            png_color_8p sig_bit_p;
            png_get_sBIT(_png_ptr, _info_ptr, &sig_bit_p);
            png_set_shift(_png_ptr, sig_bit_p);
        }
        if ((color_type & PNG_COLOR_MASK_COLOR) != 0)
        {
            png_set_bgr(_png_ptr);
        }
        png_set_filler(_png_ptr, 0xFF, PNG_FILLER_AFTER);
        png_read_update_info(_png_ptr, _info_ptr);

        dimensions.upperLeftPoint.x = 0;
        dimensions.upperLeftPoint.y = 0;
        dimensions.width            = cWidth;
        dimensions.height           = cHeight;
        return true;
    }

    bool read(gsl::span<BGRAPixel> buffer) noexcept
    {
        std::vector<png_bytep> row_pointers{};
        row_pointers.resize(dimensions.height);

        for (auto row = 0U; row < dimensions.height; row++)
        {
            row_pointers[row] = (png_bytep)png_malloc(_png_ptr, png_get_rowbytes(_png_ptr, _info_ptr));
        }
        png_read_image(_png_ptr, row_pointers.data());
        png_read_end(_png_ptr, _info_ptr);
        for (size_t row = 0U; row < dimensions.height; row++)
        {
            memcpy(&(buffer[row * dimensions.width]), row_pointers[row], 4ULL /*32 / 8*/ * dimensions.width);
        }

        for (auto row = 0U; row < dimensions.height; row++)
        {
            png_free(_png_ptr, row_pointers[row]);
        }
        return true;
    }

    void deinit() noexcept
    {
        if (_png_ptr != nullptr)
        {
            png_destroy_read_struct(&_png_ptr, &_info_ptr, nullptr);
            _png_ptr = nullptr;
        }
        if (_pngFile != nullptr)
        {
            fclose(_pngFile);
            _pngFile = nullptr;
        }
    }

    Rectangle dimensions{};

    FILE *_pngFile{};

    png_structp _png_ptr{};

    png_infop _info_ptr{};
};

Reader::Reader(std::string_view const filepath) noexcept
{
    Logger::globalInstance().addProject<ReaderProject>();
    _impl.init(filepath);
}

Reader::~Reader() noexcept { deinit(); }

bool Reader::fileTypeFits() noexcept { return _impl->fileTypeFits(); }

bool Reader::imagePresent() const noexcept { return _impl->imagePresent(); }

bool Reader::init() noexcept { return _impl->init(); }

Rectangle Reader::dimensions() const noexcept { return _impl->dimensions; }

bool Reader::read(gsl::span<BGRAPixel> buffer) noexcept { return _impl->read(buffer); }

void Reader::deinit() noexcept { _impl->deinit(); }

} // namespace Terrahertz::PNG
