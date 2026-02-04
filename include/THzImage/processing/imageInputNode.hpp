#ifndef THZ_IMAGE_PROCESSING_IMAGEINPUTNODE_HPP
#define THZ_IMAGE_PROCESSING_IMAGEINPUTNODE_HPP

#include "THzImage/common/image.hpp"
#include "THzImage/processing/iNode.hpp"

namespace Terrahertz::ImageProcessing {

/// @brief Node for putting a single image into the processing chain.
/// This can be used to perform processing on a single image
/// or to manipulate the image between processing runsssss.
class ImageInputNode : public INode<BGRAPixel>
{
public:
    /// @brief Initializes a new ImageInputNode.
    ImageInputNode() noexcept = default;

    /// @brief Explicitly deleted to prevent copy construction.
    ImageInputNode(ImageInputNode const &) noexcept = delete;

    /// @brief Explicitly deleted to prevent move construction.
    ImageInputNode(ImageInputNode &&other) noexcept = delete;

    /// @brief Explicitly deleted to prevent copy assignment.
    ImageInputNode &operator=(ImageInputNode const &other) noexcept = delete;

    /// @brief Explicitly deleted to prevent move assignment.
    ImageInputNode &operator=(ImageInputNode &&other) noexcept = delete;

    /// @brief Finalizes this instance, performing a deinit.
    ~ImageInputNode() noexcept = default;

    /// @brief Triggers loading the next image.
    ///
    /// @param countFailure If true, failures increase the count as well.
    /// @return True if the next image was loaded, false otherwise.
    [[nodiscard]] bool next(bool const countFailure = false) noexcept override;

    /// @copydoc INode::toCount
    [[nodiscard]] ToCountResult toCount(size_t const target, bool const force = false) noexcept override;

    /// @copydoc INode::operator[]
    [[nodiscard]] ImageType &operator[](size_t const index) noexcept override;

    /// @copydoc INode::slots
    [[nodiscard]] size_t slots() const noexcept override;

    /// @copydoc INode::count
    [[nodiscard]] size_t count() const noexcept override;

    /// @brief Provides access to the wrapped image.
    ///
    /// @return Reference to the wrapped image to manipulate the image.
    BGRAImage &image() noexcept;

private:
    /// @brief The image wrapped by the node.
    BGRAImage _image{};

    /// @brief Counts how often next() has been called.
    size_t _counter{};
};

} // namespace Terrahertz::ImageProcessing

#endif // !THZ_IMAGE_PROCESSING_IMAGEINPUTNODE_HPP
