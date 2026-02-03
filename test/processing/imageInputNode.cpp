#include "THzImage/processing/imageInputNode.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct ProcessingImageInputNode : public testing::Test
{
    using SutClass = ImageProcessing::ImageInputNode;
};

TEST_F(ProcessingImageInputNode, BaseBehavior)
{
    BGRAImage const expectedImage{};

    SutClass sut{};
    EXPECT_EQ(sut.slots(), 1U);
    EXPECT_EQ(sut.count(), 0U);
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut.count(), 1U);
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut.count(), 2U);
    EXPECT_EQ(sut[0U], expectedImage);
    EXPECT_EQ(sut[1U], expectedImage);
    EXPECT_EQ(sut[2U], expectedImage);
}

TEST_F(ProcessingImageInputNode, ImageReferencesMatch)
{
    SutClass sut{};
    EXPECT_EQ(&(sut.image()), &(sut[0U]));

    Rectangle const dimensions{16U, 16U};
    EXPECT_TRUE(sut.image().setDimensions(dimensions));
    EXPECT_EQ(sut[0U].dimensions(), dimensions);
}

} // namespace Terrahertz::UnitTests
