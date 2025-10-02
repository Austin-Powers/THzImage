#ifndef THZ_IMAGE_PROCESSING_INODE_HPP
#define THZ_IMAGE_PROCESSING_INODE_HPP

#include "THzImage/common/image.hpp"

namespace Terrahertz::ImageProcessing::Internal {

/// @brief The interface used by all image processing nodes.
///
/// @tparam TPixelType The type of pixel used by the images returned by the node.
template <typename TPixelType>
class INode
{
public:
    /// @brief The type of image returned by the node.
    using ImageType = Image<TPixelType>;

    virtual ~INode() noexcept {};

    /// @brief Triggers the node to process the next image.
    ///
    /// @return True if processing was successful, false otherwise.
    [[nodiscard]] virtual bool next() noexcept = 0;

    /// @brief Access the images stored by the node.
    ///
    /// @param index The index of the image, 0 is the newest image.
    /// @return The image for the given index, empty if indes is out of range.
    /// @remark Returns an empty image if index is out of range.
    [[nodiscard]] virtual ImageType &operator[](size_t const index) noexcept = 0;

    /// @brief Returns the number of slots of the node.
    ///
    /// @return The number of slots of the node.
    [[nodiscard]] virtual size_t slots() const noexcept = 0;
};

} // namespace Terrahertz::ImageProcessing::Internal

#endif // !THZ_IMAGE_PROCESSING_INODE_HPP
