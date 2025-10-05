#ifndef THZ_IMAGE_PROCESSING_TESTINPUTNODE_HPP
#define THZ_IMAGE_PROCESSING_TESTINPUTNODE_HPP

#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/image.hpp"
#include "THzImage/processing/iNode.hpp"

namespace Terrahertz::ImageProcessing {

/// @brief Node for inserting test images into the node system.
class TestInputNode : public Internal::INode<BGRAPixel>
{
public:
    /// @brief Initializes a new TestInputNode.
    TestInputNode(Rectangle const &dimensions) noexcept;

    /// @brief Explicitly deleted to prevent copy construction.
    TestInputNode(TestInputNode const &) noexcept = delete;

    /// @brief Explicitly deleted to prevent move construction.
    TestInputNode(TestInputNode &&other) noexcept = delete;

    /// @brief Explicitly deleted to prevent copy assignment.
    TestInputNode &operator=(TestInputNode const &other) noexcept = delete;

    /// @brief Explicitly deleted to prevent move assignment.
    TestInputNode &operator=(TestInputNode &&other) noexcept = delete;

    /// @brief Finalizes this instance, performing a deinit.
    ~TestInputNode() noexcept = default;

    /// @brief Triggers loading the next image.
    ///
    /// @return True if the next image was loaded, false otherwise.
    [[nodiscard]] bool next() noexcept override;

    /// @copydoc INode::operator[]
    [[nodiscard]] ImageType &operator[](size_t const index) noexcept override;

    /// @copydoc INode::slots
    [[nodiscard]] size_t slots() const noexcept override;

    /// @copydoc INode::count
    [[nodiscard]] size_t count() const noexcept override;

private:
    /// @brief The image to make available.
    BGRAImage _image{};
};

} // namespace Terrahertz::ImageProcessing

#endif // !THZ_IMAGE_PROCESSING_TESTINPUTNODE_HPP
