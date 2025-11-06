#include "THzImage/processing/transformerNode.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

class TestTransformer : public IImageTransformer<BGRAPixel>
{
public:
    TestTransformer(std::uint8_t const value) noexcept : _value{value} {}

    Rectangle dimensions() const noexcept override { return {10U, 10U}; }

    bool transform(MyPixelType &pixel) noexcept override
    {
        pixel.blue = _value;
        return true;
    }

    bool skip() noexcept override { return true; }

    bool reset() noexcept override { return true; }

    bool nextImage() noexcept override
    {
        ++_value;
        return true;
    }

private:
    std::uint8_t _value;
};

using TestTransformerNode = ImageProcessing::Internal::TransformerNode<BGRAPixel, TestTransformer>;

struct ProcessingTransformerNode : public testing::Test
{};

TEST_F(ProcessingTransformerNode, BaseBehavior)
{
    std::uint8_t const  value{16U};
    TestTransformerNode sut{2U, true, value};
    EXPECT_EQ(sut.slots(), 2U);
    EXPECT_EQ(sut.count(), 0U);
    EXPECT_EQ(sut[2U].dimensions(), Rectangle{});
}

TEST_F(ProcessingTransformerNode, TransformerWrappingCorrect)
{
    std::uint8_t const value{16U};
    Rectangle const    expectedDimensions{10U, 10U};

    TestTransformerNode sut{2U, true, value};

    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut[0U].dimensions(), expectedDimensions);
    EXPECT_EQ(sut[0U][0U].blue, value);
    EXPECT_EQ(sut.count(), 1U);
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut[0U].dimensions(), expectedDimensions);
    EXPECT_EQ(sut[0U][0U].blue, value + 1U);
    EXPECT_EQ(sut.count(), 2U);
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut[0U].dimensions(), expectedDimensions);
    EXPECT_EQ(sut[0U][0U].blue, value + 2U);
    EXPECT_EQ(sut.count(), 3U);
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut[0U].dimensions(), expectedDimensions);
    EXPECT_EQ(sut[0U][0U].blue, value + 3U);
    EXPECT_EQ(sut.count(), 4U);
}

TEST_F(ProcessingTransformerNode, ForwardNextSetToFalse)
{
    std::uint8_t const value{16U};
    Rectangle const    expectedDimensions{10U, 10U};

    TestTransformerNode sut{2U, false, value};

    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut[0U].dimensions(), expectedDimensions);
    EXPECT_EQ(sut[0U][0U].blue, value);
    EXPECT_EQ(sut.count(), 1U);
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut[0U].dimensions(), expectedDimensions);
    EXPECT_EQ(sut[0U][0U].blue, value);
    EXPECT_EQ(sut.count(), 2U);
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut[0U].dimensions(), expectedDimensions);
    EXPECT_EQ(sut[0U][0U].blue, value);
    EXPECT_EQ(sut.count(), 3U);
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut[0U].dimensions(), expectedDimensions);
    EXPECT_EQ(sut[0U][0U].blue, value);
    EXPECT_EQ(sut.count(), 4U);
}

TEST_F(ProcessingTransformerNode, ToCount)
{
    Rectangle const defaultRectangle{};

    std::uint8_t const  value{16U};
    TestTransformerNode sut{2U, false, value};

    EXPECT_EQ(sut.toCount(0U), TestTransformerNode::ToCountResult::NotUpdated);
    EXPECT_EQ(sut[0U].dimensions(), defaultRectangle);
    EXPECT_EQ(sut[1U].dimensions(), defaultRectangle);

    EXPECT_EQ(sut.toCount(2U), TestTransformerNode::ToCountResult::Updated);
    auto const dim0 = sut[0U].dimensions();
    auto const dim1 = sut[1U].dimensions();
    EXPECT_NE(dim0, defaultRectangle);
    EXPECT_NE(dim1, defaultRectangle);

    EXPECT_EQ(sut.toCount(2U), TestTransformerNode::ToCountResult::NotUpdated);
    EXPECT_EQ(sut[0U].dimensions(), dim0);
    EXPECT_EQ(sut[1U].dimensions(), dim1);

    EXPECT_EQ(sut.toCount(1U), TestTransformerNode::ToCountResult::Ahead);
    EXPECT_EQ(sut[0U].dimensions(), dim0);
    EXPECT_EQ(sut[1U].dimensions(), dim1);
}

} // namespace Terrahertz::UnitTests
