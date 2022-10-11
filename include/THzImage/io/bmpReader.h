#ifndef THZ_IMAGE_IO_BMPREADER_H
#define THZ_IMAGE_IO_BMPREADER_H

#include "THzImage/common/iImageReader.h"
#include "THzImage/common/pixel.h"

#include <string_view>

namespace Terrahertz {

class BMPReader : public IImageReader<BGRAPixel>
{
public:
    /// @brief Initializes a new BMPReader.
    ///
    /// @param filepath The path of the file to read from.
    BMPReader(std::string_view const filepath) noexcept;

    /// @copydoc IImageReader::multipleImages
    bool multipleImages() const noexcept override;

    /// @copydoc IImageReader::init
    bool init() noexcept override;

    /// @copydoc IImageReader::dimensions
    Rectangle dimensions() const noexcept override;

    /// @copydoc IImageReader::read
    bool read(gsl::span<BGRAPixel> buffer) noexcept override;

    /// @copydoc IImageReader::deinit
    void deinit() noexcept;
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_IO_BMPREADER_H
