#ifndef THZ_TEST_IMAGE_PROCESSING_MOCKNODE_HPP
#define THZ_TEST_IMAGE_PROCESSING_MOCKNODE_HPP

#include "THzImage/processing/iNode.hpp"

namespace Terrahertz::UnitTests {

/// @brief Enables mocking
///
/// @tparam TPixelType
template <Pixel TPixelType>
struct MockNode : public ImageProcessing::Internal::INode<TPixelType>
{
    using MyImageType = Image<TPixelType>;

    Image<TPixelType> image{};

    /// @copydoc INode::next
    [[nodiscard]] bool next(bool const countFailure = false) noexcept override { return false; }

    /// @copydoc INode::toCount
    [[nodiscard]] ImageProcessing::ToCountResult toCount(size_t const target, bool force = false) noexcept override
    {
        return ImageProcessing::ToCountResult::Failure;
    }

    /// @copydoc INode::operator[]
    [[nodiscard]] MyImageType &operator[](size_t const index) noexcept override { return image; }

    /// @copydoc INode::slots
    [[nodiscard]] size_t slots() const noexcept override { return 0; }

    /// @copydoc INode::count
    [[nodiscard]] size_t count() const noexcept override { return 0; }
};

} // namespace Terrahertz::UnitTests

#endif // !THZ_TEST_IMAGE_PROCESSING_MOCKNODE_HPP
