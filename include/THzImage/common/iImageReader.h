#ifndef THZ_IMAGE_COMMON_IIMAGEREADER_H
#define THZ_IMAGE_COMMON_IIMAGEREADER_H

#include "THzCommon/math/rectangle.h"
#include "pixel.h"

#include <gsl/gsl>

namespace Terrahertz {

/// @brief Interface for all classes reading images.
///
/// @tparam TPixelType The pixel type of the reader.
template <typename TPixelType>
class IImageReader
{
public:
    static_assert(is_pixel_type<TPixelType>::value, "TPixelType is not a known pixel type");

    /// @brief Shortcut to the used pixel type.
    using PixelType = TPixelType;

    /// @brief Default the destructor to make it virtual.
    virtual ~IImageReader() noexcept {}

    /// @brief If true the reader will read a different image every time the read cycle is executed.
    ///
    /// @return True if the read image changes for every read call, false otherwise.
    virtual bool multipleImages() const noexcept = 0;

    /// @brief Is called by the image to initalize the reading process.
    ///
    /// @return True if init was successful, false otherwise.
    virtual bool init() noexcept = 0;

    /// @brief Returns the dimensions of the image to read.
    ///
    /// @return The dimensions of the image to read.
    virtual Rectangle dimensions() const noexcept = 0;

    /// @brief Reads the image data into the given buffer.
    ///
    /// @param buffer The buffer to read the pixel data into.
    /// @return True if reading was successful, false otherwise.
    virtual bool read(gsl::span<TPixelType> buffer) noexcept = 0;

    /// @brief Is called by the image at the end of the reading process.
    ///
    /// @remarks This method is called regardless of success or failure of reading.
    virtual void deinit() noexcept = 0;
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_COMMON_IIMAGEREADER_H
