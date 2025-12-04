#ifndef THZ_IMAGE_PROCESSING_EASYWRITER_HPP
#define THZ_IMAGE_PROCESSING_EASYWRITER_HPP

#include "THzImage/common/pixel.hpp"
#include "THzImage/io/imageSeriesWriter.hpp"
#include "THzImage/io/pngWriter.hpp"
#include "THzImage/processing/iNode.hpp"

#include <filesystem>

namespace Terrahertz::ImageProcessing {

/// @brief Writes images directly to enumerated PNG-files for debugging or storage purposes.
class EasyWriter
{
public:
    /// @brief Initializes a new EasyWriter instance.
    ///
    /// @param filepath The path for the files, has to contain a single '?' to signify where the numbering shall go.
    EasyWriter(std::filesystem::path const filepath) noexcept;

    /// @brief Writes the given image to a PNG file.
    ///
    /// @tparam TPixelType The type of pixel used by the image.
    /// @param image The image to write.
    /// @return True if the image was written, false otherwise.
    template <Pixel TPixelType>
    [[nodiscard]] bool write(Image<TPixelType> const &image) noexcept
    {
        if (_bgraCopy.setDimensions(image.dimensions()))
        {
            auto const pixels = image.dimensions().area();
            for (auto i = 0U; i < pixels; ++i)
            {
                _bgraCopy[i] = image[i];
            }
            return write(_bgraCopy);
        }
        return false;
    }

    /// @brief Writes the given BGRAImage to a PNG file.
    ///
    /// @param image The image to write.
    /// @return True if the image was written, false otherwise.
    template <>
    [[nodiscard]] bool write(Image<BGRAPixel> const &image) noexcept;

private:
    /// @brief The PNG image series writer used to write the images.
    std::optional<ImageSeries::Writer<PNG::Writer>> _writer{};

    /// @brief BGRA copy of a non BGRA image given to the writer.
    Image<BGRAPixel> _bgraCopy{};
};

} // namespace Terrahertz::ImageProcessing

#endif // !THZ_IMAGE_PROCESSING_NODEWRITER_HPP
