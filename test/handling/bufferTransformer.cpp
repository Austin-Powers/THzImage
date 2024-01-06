#include "THzImage/handling/bufferTransformer.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/common/pixel.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <cstdint>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct Handling_BufferTransformer : public testing::Test
{
    TestImageGenerator generator{Rectangle{4U, 4U}};

    ImageRingBuffer<BGRAPixel> buffer{generator, 3U};

    BufferTransformer<BGRAPixel> sut{};
};

TEST_F(Handling_BufferTransformer, DefaultConstructedBehavesLikeDefaultView)
{
    BGRAPixel pixel{};

    ImageView<BGRAPixel> defaultView{};
    EXPECT_EQ(sut.dimensions(), defaultView.dimensions());
    EXPECT_EQ(sut.reset(), defaultView.reset());
    EXPECT_EQ(sut.nextImage(), defaultView.nextImage());
}

TEST_F(Handling_BufferTransformer, UpdateRejectsInvalidValues)
{
    EXPECT_FALSE(sut.update(buffer, 3U, true));
    EXPECT_FALSE(sut.update(buffer, 4U, false));
}

TEST_F(Handling_BufferTransformer, UpdateWithValidValuesInteractsWithGivenBuffer)
{
    EXPECT_TRUE(buffer.next());
    EXPECT_EQ(buffer.count(), 1U);
    EXPECT_TRUE(sut.update(buffer, 0U, false));
    BGRAImage target{};
    EXPECT_TRUE(target.executeAndIngest(sut));

    for (auto i = 0U; i < buffer[0U].dimensions().area(); ++i)
    {
        EXPECT_EQ(target[i], buffer[0U][i]);
    }
    EXPECT_EQ(buffer.count(), 1U);
}

TEST_F(Handling_BufferTransformer, NextImageCallingNextOnTheBuffer)
{
    EXPECT_TRUE(buffer.next());
    EXPECT_EQ(buffer.count(), 1U);
    EXPECT_TRUE(sut.update(buffer, 0U, false));
    EXPECT_FALSE(sut.nextImage());
    EXPECT_EQ(buffer.count(), 1U);
    EXPECT_TRUE(sut.update(buffer, 0U, true));
    EXPECT_TRUE(sut.nextImage());
    EXPECT_EQ(buffer.count(), 2U);
}

} // namespace Terrahertz::UnitTests
