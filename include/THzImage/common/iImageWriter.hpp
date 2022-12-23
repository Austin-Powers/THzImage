#ifndef THZ_IMAGE_COMMON_IIMAGEWRITER_H
#define THZ_IMAGE_COMMON_IIMAGEWRITER_H

#include "THzCommon/math/rectangle.hpp"
#include "pixel.h"

#include <gsl/gsl>

namespace Terrahertz {

/// @brief Interface for all classes writing images.
///
/// @tparam TPixelType The pixel type of the writer.
template <typename TPixelType>
class IImageWriter
{
public:
    static_assert(is_pixel_type<TPixelType>::value, "TPixelType is not a known pixel type");

    /// @brief Shortcut to the used pixel type.
    using PixelType = TPixelType;

    /// @brief Default the destructor to make it virtual.
    virtual ~IImageWriter() noexcept {}

    /// @brief Is called by the image to initialize the writing process.
    ///
    /// @return True if init was successful, false otherwise.
    virtual bool init() noexcept = 0;

    /// @brief Writes the iamge data of the given buffer.
    ///
    /// @param dimensions The dimensions of the image.
    /// @param buffer The buffer of image data to write.
    /// @return True if writing was successful, false otherwise.
    virtual bool write(Rectangle const &dimensions, gsl::span<TPixelType const> const buffer) noexcept = 0;

    /// @brief Is called by the image at the end of the writing process.
    ///
    /// @remarks This method is called regardless of success or failure of writing.
    virtual void deinit() noexcept = 0;
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_COMMON_IIMAGEWRITER_H
