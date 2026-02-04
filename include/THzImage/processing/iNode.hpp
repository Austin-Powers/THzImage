#ifndef THZ_IMAGE_PROCESSING_INODE_HPP
#define THZ_IMAGE_PROCESSING_INODE_HPP

#include "THzImage/common/image.hpp"

namespace Terrahertz::ImageProcessing {

/// @brief The different results that can be produced by the toCount method of an node.
enum class ToCountResult
{
    /// @brief Node was updated to the specified counter.
    Updated,

    /// @brief Node was already at the specified counter.
    NotUpdated,

    /// @brief Node is ahead of the specified counter.
    Ahead,

    /// @brief Processing next image failed.
    Failure
};

/// @brief The interface used by all image processing nodes.
///
/// @tparam TPixelType The type of pixel used by the images returned by the node.
template <Pixel TPixelType>
class INode
{
public:
    /// @brief The type of image returned by the node.
    using ImageType = Image<TPixelType>;

    virtual ~INode() noexcept {};

    /// @brief Triggers the node to process the next image.
    ///
    /// @param countFailure If true, failures increase the count as well.
    /// @return True if processing was successful, false otherwise.
    [[nodiscard]] virtual bool next(bool const countFailure = false) noexcept = 0;

    /// @brief Calls next until the target count is reached.
    ///
    /// @param target The count at which to stop calling next.
    /// @param force If true, count will be reached even if failures occur.
    /// @return True if count was reached, false if the node already overstepped count.
    /// @remark Nodes in a chain can call this method so their count stays in sync with other nodes.
    [[nodiscard]] virtual ToCountResult toCount(size_t const target, bool const force = false) noexcept = 0;

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

    /// @brief Returns the number of images processed by the node.
    ///
    /// @return The number of images processed by the node.
    [[nodiscard]] virtual size_t count() const noexcept = 0;
};

} // namespace Terrahertz::ImageProcessing

#endif // !THZ_IMAGE_PROCESSING_INODE_HPP
