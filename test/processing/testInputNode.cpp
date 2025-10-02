#include "THzImage/processing/testInputNode.hpp"

#include "THzImage/io/testImageGenerator.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTest {

struct ProcessingTestInputNode : public testing::Test
{};

TEST_F(ProcessingTestInputNode, CorrectBehavior)
{
    Rectangle const dimensions{16U, 16U};

    TestImageGenerator generator{dimensions};
    BGRAImage          expectedImage{};
    ASSERT_TRUE(expectedImage.readFrom(generator));

    ImageProcessing::TestInputNode sut{Rectangle{16U, 16U}};
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut.slots(), 1U);
    EXPECT_EQ(sut[0U], expectedImage);
    EXPECT_EQ(sut[1U], expectedImage);
    EXPECT_EQ(sut[2U], expectedImage);
}

} // namespace Terrahertz::UnitTest
