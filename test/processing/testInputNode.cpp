#include "THzImage/processing/testInputNode.hpp"

#include "THzImage/io/testImageGenerator.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct ProcessingTestInputNode : public testing::Test
{
    using SutClass = ImageProcessing::TestInputNode;
};

TEST_F(ProcessingTestInputNode, CorrectBaseBehavior)
{
    Rectangle const dimensions{16U, 16U};

    TestImageGenerator generator{dimensions};
    BGRAImage          expectedImage{};
    ASSERT_TRUE(expectedImage.readFrom(generator));

    SutClass sut{dimensions};
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

TEST_F(ProcessingTestInputNode, ZeroDimensions)
{
    Rectangle const dimensions{};
    SutClass        sut{dimensions};
    EXPECT_EQ(sut[0].dimensions(), dimensions);
}

TEST_F(ProcessingTestInputNode, ToCount)
{
    Rectangle const dimensions{16U, 16U};
    SutClass        sut{dimensions};

    EXPECT_EQ(sut.toCount(4U), ImageProcessing::ToCountResult::Updated);
    EXPECT_EQ(sut.count(), 4U);
    EXPECT_EQ(sut.toCount(2U), ImageProcessing::ToCountResult::Ahead);
    EXPECT_EQ(sut.count(), 4U);
    EXPECT_EQ(sut.toCount(4U), ImageProcessing::ToCountResult::NotUpdated);
    EXPECT_EQ(sut.count(), 4U);
}

TEST_F(ProcessingTestInputNode, BoolsInNextAndToCountAreIgnored)
{
    Rectangle const dimensions{16U, 16U};
    SutClass        sut{dimensions};
    EXPECT_TRUE(sut.next(true));
    EXPECT_EQ(sut.count(), 1U);
    EXPECT_EQ(sut.toCount(2U, true), ImageProcessing::ToCountResult::Updated);
}

} // namespace Terrahertz::UnitTests
