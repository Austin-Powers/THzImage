#ifndef THZ_IMAGE_IO_PNGREADER_H
#define THZ_IMAGE_IO_PNGREADER_H

#include "THzCommon/utility/staticPImpl.hpp"
#include "THzImage/common/iImageReader.h"
#include "THzImage/common/pixel.h"

#include <cstdint>
#include <string_view>

namespace Terrahertz::PNG {

/// @brief Reads an image from a file using the Portable-Network-Graphics format.
class Reader : public IImageReader<BGRAPixel>
{
public:
    /// @brief Initializes a new PNG::Reader.
    ///
    /// @param filepath The path of the file to read from.
    Reader(std::string_view const filepath) noexcept;

    /// @brief Explicitly deleted to prevent copy construction.
    Reader(Reader const &other) noexcept = delete;

    /// @brief Explicitly deleted to prevent move construction.
    Reader(Reader &&other) noexcept = delete;

    /// @brief Explicitly deleted to prevent copy assignment.
    Reader &operator=(Reader const &other) noexcept = delete;

    /// @brief Explicitly deleted to prevent move assignment.
    Reader &operator=(Reader &&other) noexcept = delete;

    /// @brief Finalizes this instance, performing a deinit.
    ~Reader() noexcept;

    /// @copydoc IImageReader::multipleImages
    bool multipleImages() const noexcept override;

    /// @copydoc IImageReader::init
    bool init() noexcept override;

    /// @copydoc IImageReader::dimensions
    Rectangle dimensions() const noexcept override;

    /// @copydoc IImageReader::read
    bool read(gsl::span<BGRAPixel> buffer) noexcept override;

    /// @copydoc IImageReader::deinit
    void deinit() noexcept override;

private:
    /// @brief Forward declaration of the implementation.
    struct Impl;

    /// @brief Pointer to the implementation.
    StaticPImpl<Impl, 40U> _impl{};
};

} // namespace Terrahertz::PNG

#endif // !THZ_IMAGE_IO_PNGREADER_H
