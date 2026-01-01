#ifndef THZ_IMAGE_COMMON_IIMAGEREADER_HPP
#define THZ_IMAGE_COMMON_IIMAGEREADER_HPP

#include "THzCommon/math/rectangle.hpp"
#include "pixel.hpp"

#include <concepts>
#include <filesystem>
#include <gsl/gsl>

namespace Terrahertz {

/// @brief Forward declaration of Image for convenience function readInto.
/// @tparam TPixelType The type of pixel used by the image.
template <Pixel TPixelType>
class Image;

/// @brief Interface for all classes reading images.
///
/// @tparam TPixelType The pixel type of the reader.
template <Pixel TPixelType>
class IImageReader
{
public:
    /// @brief Shortcut to the used pixel type.
    using PixelType = TPixelType;

    /// @brief Default the destructor to make it virtual.
    virtual ~IImageReader() noexcept {}

    /// @brief Reads into the given buffer.
    ///
    /// @param buffer The buffer to put the read data into.
    /// @return True if reading was successful, false otherwise.
    [[nodiscard]] bool readInto(Image<TPixelType> &buffer) noexcept { return buffer.readFrom(*this); }

    /// @brief Checks if executing the read cycle will return an image or if reader is already exhausted.
    ///
    /// @return True if there is an image to read, false otherwise.
    virtual bool imagePresent() const noexcept = 0;

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

// clang-format off

/// @brief Concept of a ImageReader.
template<typename TReaderType>
concept ImageReader = requires
{
    // first check if the class has a PixelType member
    typename TReaderType::PixelType;

    // now use the PixelType member to check if TReaderType implements IImageReader
    std::is_base_of_v<IImageReader<typename TReaderType::PixelType>, TReaderType>;
};

/// @brief Concept of a FileImageReader.
template<typename TFileReaderType>
concept FileImageReader = requires(TFileReaderType reader)
{
    // check if class is an ImageReader
    requires ImageReader<TFileReaderType>;

    // check if ImageReader can be constructed using a std::filesystem::path
    TFileReaderType(std::filesystem::path{});

    // check if a method for determining if can be read by the given reader is present
    // this method should enable checking if the file can be opened without flooding the log with error messages
    {reader.fileTypeFits()} -> std::same_as<bool>;
};

// clang-format on

} // namespace Terrahertz

#endif // !THZ_IMAGE_COMMON_IIMAGEREADER_HPP
