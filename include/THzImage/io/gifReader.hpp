#ifndef THZ_IMAGE_IO_GIFREADER_HPP
#define THZ_IMAGE_IO_GIFREADER_HPP

#include "THzImage/common/iImageReader.hpp"
#include "THzImage/common/pixel.hpp"

namespace Terrahertz::GIF {

/// @brief Reads an image from a file using the GIF format.
class Reader : public IImageReader<BGRAPixel>
{
public:
    using IImageReader::readInto;

    /// @brief Initializes a new BMP::Reader.
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
};

} // namespace Terrahertz::GIF

#endif // !THZ_IMAGE_IO_GIFREADER_HPP
