#ifndef THZ_IMAGE_IO_TESTIMAGEGENERATOR_HPP
#define THZ_IMAGE_IO_TESTIMAGEGENERATOR_HPP

#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/iImageReader.hpp"
#include "THzImage/common/pixel.hpp"

namespace Terrahertz {

/// @brief Reader for generating a test image.
class TestImageGenerator : public IImageReader<BGRAPixel>
{
public:
    /// @brief Initializes a new TestImageGenerator for the given values.
    ///
    /// @param dimensions The dimensions of the test image.
    TestImageGenerator(Rectangle const &dimensions) noexcept;

    /// @copydoc IImageReader::imagePresent
    [[nodiscard]] bool imagePresent() const noexcept override;

    /// @copydoc IImageReader::init
    [[nodiscard]] bool init() noexcept override;

    /// @copydoc IImageReader::dimensions
    [[nodiscard]] Rectangle dimensions() const noexcept override;

    /// @copydoc IImageReader::read
    [[nodiscard]] bool read(gsl::span<BGRAPixel> buffer) noexcept override;

    /// @copydoc IImageReader::deinit
    void deinit() noexcept override;

private:
    /// @brief The dimensions of the test image.
    Rectangle _dimensions{};
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_IO_TESTIMAGEGENERATOR_HPP
