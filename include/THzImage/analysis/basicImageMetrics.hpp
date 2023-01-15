#ifndef THZ_IMAGE_ANALYSIS_BASICIMAGEMETRICS_HPP
#define THZ_IMAGE_ANALYSIS_BASICIMAGEMETRICS_HPP

#include "THzImage/common/iImageWriter.hpp"
#include "THzImage/common/pixel.hpp"

namespace Terrahertz {

/// @brief Calculates a basic set of metrics of the given image.
class BasicImageMetrics : public IImageWriter<BGRAPixel>
{
public:
    /// @brief Initializes a new BasicImageMetrics.
    ///
    /// @param filepath The path to write the results to.
    BasicImageMetrics(std::string_view const filepath) noexcept;

    /// @copydoc IImageWriter::init
    bool init() noexcept override;

    /// @copydoc IImageWriter::write
    bool write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept override;

    /// @copydoc IImageWriter::deinit
    void deinit() noexcept override;

private:
    /// @brief The path to write the BMP-File to.
    std::string_view const _filepath;
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_ANALYSIS_BASICIMAGEMETRICS_HPP
