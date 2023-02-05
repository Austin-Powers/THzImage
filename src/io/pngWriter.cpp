#include "THzImage/io/pngWriter.hpp"

#include "THzCommon/logging/logging.hpp"

#include <cstdio>
#include <png.h>

namespace Terrahertz::PNG {

/// @brief Name provider for the THzImage.IO.PNG.Writer class.
struct WriterProject
{
    static constexpr char const *name() noexcept { return "THzImage.IO.PNG.Writer"; }
};

Writer::Writer(std::filesystem::path const filepath) noexcept : _filepath{filepath} {}

bool Writer::init() noexcept { return true; }

bool Writer::write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept
{
    if (dimensions.area() != buffer.size())
    {
        logMessage<LogLevel::Error, WriterProject>("Image dimensions do not match the given buffer size");
        return false;
    }

    FILE       *pngFile;
    png_structp png_ptr;
    png_infop   info_ptr;

#ifdef _WIN32
    _wfopen_s(&pngFile, _filepath.c_str(), L"wb");
#else
    pngFile = fopen(_filepath.c_str(), "wb");
#endif
    if (pngFile == NULL)
    {
        return false;
    }
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        fclose(pngFile);
        return false;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(pngFile);
        png_destroy_write_struct(&png_ptr, NULL);
        return false;
    }
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        /* If we get here, we had a problem writing the file */
        fclose(pngFile);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    }
    png_init_io(png_ptr, pngFile);
    png_set_IHDR(png_ptr,
                 info_ptr,
                 dimensions.width,
                 dimensions.height,
                 8,
                 PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    // write
    png_write_info(png_ptr, info_ptr);
    png_set_bgr(png_ptr);
    if (dimensions.height > PNG_UINT_32_MAX / (sizeof(png_bytep)))
    {
        png_error(png_ptr, "Image is too tall to process in memory");
    }

    auto linePtr = new BGRAPixel const *[dimensions.height];
    for (auto index = 0ULL; index < dimensions.height; ++index)
    {
        linePtr[index] = &buffer[index * dimensions.width];
    }
    png_write_image(png_ptr, (png_bytepp)linePtr);
    png_write_end(png_ptr, info_ptr);
    delete[] linePtr;

    // clean up
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(pngFile);
    return true;
}

void Writer::deinit() noexcept {}

} // namespace Terrahertz::PNG
