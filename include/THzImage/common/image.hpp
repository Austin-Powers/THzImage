#ifndef THZ_IMAGE_COMMON_IMAGE_HPP
#define THZ_IMAGE_COMMON_IMAGE_HPP

#include "THzCommon/logging/logging.hpp"
#include "THzCommon/math/rectangle.hpp"
#include "THzCommon/utility/spanhelpers.hpp"
#include "iImageReader.hpp"
#include "iImageTransformer.hpp"
#include "iImageWriter.hpp"
#include "imageView.hpp"
#include "pixel.hpp"

#include <cstddef>
#include <vector>

namespace Terrahertz {

/// @brief Name provider for the THzImage.Common.Image class.
struct ImageProject
{
    static constexpr char const *name() noexcept { return "THzImage.Common.Image"; }
};

/// @brief Class representing raster based images.
///
/// @tparam TPixelType The type of pixel used by the image.
/// @remarks Pixels are stored Left to Right, Top to Bottom.
template <typename TPixelType>
class Image
{
public:
    static_assert(is_pixel_type<TPixelType>::value, "TPixelType is not a known pixel type");

    /// @brief Shortcut to the pixel type used by this image.
    using value_type = TPixelType;

    /// @brief Reference to a pixel of  this image.
    using reference = TPixelType &;

    /// @brief Reference to a pixel of  this image.
    using const_reference = TPixelType const &;

    /// @brief The size type of this image.
    using size_type = size_t;

    /// @brief Default initializes a new image.
    Image() noexcept = default;

    /// @brief Returns the dimensions of the image.
    ///
    /// @return The dimensions of the image.
    [[nodiscard]] Rectangle const &dimensions() const noexcept { return _dimensions; }

    /// @brief Sets the new dimensions of the image and resizes the internal buffer.
    ///
    /// @param dim The new dimensions of the image.
    /// @return True if the image was resized correctly, false otherwise.
    /// @remarks This operation scrambles the currently held image data.
    [[nodiscard]] bool setDimensions(Rectangle const &dim) noexcept
    {
        auto const newArea = dim.area();
        auto const oldArea = _dimensions.area();
        if (oldArea != newArea)
        {
            _data.resize(newArea);
            if (_data.size() != newArea)
            {
                return false;
            }
        }
        _dimensions = dim;
        return true;
    }

    /// @brief Index operator of the image for retrieving pixels by index.
    ///
    /// @param index The index of the pixel to return.
    /// @return The pixel at the given index.
    [[nodiscard]] reference operator[](size_type index) noexcept { return _data[index]; }

    /// @brief Index operator of the image for retrieving pixels by index.
    ///
    /// @param index The index of the pixel to return.
    /// @return The pixel at the given index.
    [[nodiscard]] const_reference operator[](size_type index) const noexcept { return _data[index]; }

    /// @brief Returns a view of the entire image.
    ///
    /// @return A view of the entire image.
    [[nodiscard]] ImageView<TPixelType> view() noexcept { return ImageView<TPixelType>{_data.data(), _dimensions}; }

    /// @brief Returns a view of the entire image.
    ///
    /// @return A view of the entire image.
    [[nodiscard]] ImageView<TPixelType const> view() const noexcept
    {
        return ImageView<TPixelType const>{_data.data(), _dimensions};
    }

    /// @brief Returns a view of the given region of the image.
    ///
    /// @param region The region of the image to create the view of.
    /// @return A view of the region of the image.
    [[nodiscard]] ImageView<TPixelType> view(Rectangle const &region) noexcept
    {
        return ImageView<TPixelType>{_data.data(), _dimensions, region};
    }

    /// @brief Returns a view of the given region of the image.
    ///
    /// @param region The region of the image to create the view of.
    /// @return A view of the region of the image.
    [[nodiscard]] ImageView<TPixelType const> view(Rectangle const &region) const noexcept
    {
        return ImageView<TPixelType const>{_data.data(), _dimensions, region};
    }

    /// @brief Calls the given transformer and stores the result in this image.
    ///
    /// @param transformer The transformer whose result to store.
    /// @return True if the image was successfully transformed, false otherwise.
    [[nodiscard]] bool storeResultOf(IImageTransformer<TPixelType> *const transformer) noexcept
    {
        if (transformer == nullptr)
        {
            logMessage<LogLevel::Error, ImageProject>("Given transformer was nullptr");
            return false;
        }
        if (!transformer->reset())
        {
            logMessage<LogLevel::Error, ImageProject>("Transformer could not be reset");
            return false;
        }
        auto const dimensions = transformer->dimensions();
        if (dimensions.area() == 0)
        {
            logMessage<LogLevel::Error, ImageProject>("Transformer has dimensions of area 0");
            return false;
        }
        if (!setDimensions(dimensions))
        {
            logMessage<LogLevel::Error, ImageProject>("Could not resize to transformer dimensions");
            return false;
        }

        auto pixel = _data.data();
        for (auto i = 0U; i < _dimensions.area(); ++i)
        {
            if (!transformer->transform(*pixel))
            {
                ++pixel;
                break;
            }
            ++pixel;
        }
        auto const transformedPixels = std::distance(_data.data(), pixel);
        return transformedPixels == _dimensions.area();
    }

    /// @brief Reads an image from the given reader.
    ///
    /// @param reader The reader to read the image data from.
    /// @return True if the image was successfully read, false otherwise.
    [[nodiscard]] bool read(IImageReader<TPixelType> *reader) noexcept
    {
        if (reader == nullptr)
        {
            logMessage<LogLevel::Error, ImageProject>("Given reader was nullptr");
            return false;
        }
        if (!reader->init())
        {
            logMessage<LogLevel::Error, ImageProject>("Init of reader failed");
            return false;
        }

        auto result = true;
        if (!setDimensions(reader->dimensions()))
        {
            logMessage<LogLevel::Error, ImageProject>("Could not resize to reader dimensions");
            result = false;
        }
        else if (!reader->read(toSpan<TPixelType>(_data)))
        {
            logMessage<LogLevel::Error, ImageProject>("Reading failed");
            result = false;
        }

        reader->deinit();
        return result;
    }

    /// @brief Writes the image to the given writer.
    ///
    /// @param writer The writer to write the image data to.
    /// @return True if the image was successfully written, false otherwise.
    [[nodiscard]] bool write(IImageWriter<TPixelType> *writer) const noexcept
    {
        if (writer == nullptr)
        {
            logMessage<LogLevel::Error, ImageProject>("Given writer was nullptr");
            return false;
        }
        if (_dimensions.area() == 0)
        {
            logMessage<LogLevel::Error, ImageProject>("Image has no data");
            return false;
        }
        if (!writer->init())
        {
            logMessage<LogLevel::Error, ImageProject>("Init of writer failed");
            return false;
        }
        auto const result = writer->write(_dimensions, toSpan<TPixelType const>(_data));
        if (!result)
        {
            logMessage<LogLevel::Error, ImageProject>("Writing failed");
        }
        writer->deinit();
        return result;
    }

private:
    /// @brief The dimensions of the image.
    Rectangle _dimensions{};

    /// @brief The memory holding the image data.
    std::vector<TPixelType> _data{};
};

/// @brief Using declaration for an image using BGRAPixel.
using BGRAImage = Image<BGRAPixel>;

/// @brief Using declaration for an image using HSVAPixel.
using BGRAImage = Image<BGRAPixel>;

} // namespace Terrahertz

#endif // !THZ_IMAGE_COMMON_IMAGE_HPP
