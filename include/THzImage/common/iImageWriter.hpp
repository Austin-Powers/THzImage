#ifndef THZ_IMAGE_COMMON_IIMAGEWRITER_HPP
#define THZ_IMAGE_COMMON_IIMAGEWRITER_HPP

#include "THzCommon/math/rectangle.hpp"
#include "pixel.hpp"

#include <concepts>
#include <filesystem>
#include <gsl/gsl>

namespace Terrahertz {

/// @brief Forward declaration of Image for convenience function writeContentOf.
/// @tparam TPixelType The type of pixel used by the image.
template <typename TPixelType>
class Image;

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

    /// @brief Writes the content of the given buffer.
    ///
    /// @param buffer The buffer whos content to write.
    /// @return True if image was writing was successful, false otherwise.
    [[nodiscard]] bool writeContentOf(Image<TPixelType> const &buffer) noexcept { return buffer.writeTo(this); }

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

// clang-format off

/// @brief Concept of a ImageWriter.
template<typename TWriterType>
concept ImageWriter = requires
{
    // first check if the class has a PixelType member
    typename TWriterType::PixelType;

    // now use the PixelType member to check if TWriterType implements IImageWriter
    std::is_base_of_v<IImageWriter<typename TWriterType::PixelType>, TWriterType>;
};

/// @brief Concept of a FileImageWriter.
template<typename TFileWriterType>
concept FileImageWriter = requires(TFileWriterType reader)
{
    // check if class is an ImageWriter
    ImageWriter<TFileWriterType>;

    // check if ImageWriter can be constructed using a std::filesystem::path
    TFileWriterType(std::filesystem::path{});
};

// clang-format on

} // namespace Terrahertz

#endif // !THZ_IMAGE_COMMON_IIMAGEWRITER_HPP
