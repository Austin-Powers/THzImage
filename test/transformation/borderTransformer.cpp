#include "THzImage/transformation/borderTransformer.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/common/imageView.hpp"
#include "THzImage/common/pixel.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct Transformation_BorderTransformer : public testing::Test
{
    class MockTransformer : public IImageTransformer<BGRAPixel>
    {
    public:
        MOCK_METHOD(Rectangle, dimensions, (), (const, noexcept, override));
        MOCK_METHOD(bool, transform, (BGRAPixel & pixel), (noexcept, override));
        MOCK_METHOD(bool, skip, (), (noexcept, override));
        MOCK_METHOD(bool, reset, (), (noexcept, override));
        MOCK_METHOD(bool, nextImage, (), (noexcept, override));
    };

    void SetUp() override
    {
        TestImageGenerator generator{baseDimensions};
        EXPECT_TRUE(image.read(generator));
        view = image.view();
    }

    Rectangle baseDimensions{4U, 4U};

    BGRAImage image{};

    BGRAImageView view{};

    Borders borders{1U, 2U, 3U, 4U};

    BGRAPixel color{0xFE, 0xAB, 0x12, 0x45};
};

TEST_F(Transformation_BorderTransformer, BordersConstructsCorrectly)
{
    EXPECT_EQ(borders.top, 1U);
    EXPECT_EQ(borders.right, 2U);
    EXPECT_EQ(borders.bottom, 3U);
    EXPECT_EQ(borders.left, 4U);
}

TEST_F(Transformation_BorderTransformer, DefaultConstructionCorrect)
{
    BorderTransformer<BGRAPixel> sut{};
    EXPECT_EQ(sut.dimensions(), Rectangle{});
    BGRAPixel const expectedPixel{0x34U, 0x11U, 0xBAU, 0xFEU};
    BGRAPixel       pixel{expectedPixel};
    EXPECT_FALSE(sut.transform(pixel));
    EXPECT_EQ(pixel, expectedPixel);
    EXPECT_FALSE(sut.skip());
    EXPECT_FALSE(sut.reset());
    EXPECT_FALSE(sut.nextImage());
}

TEST_F(Transformation_BorderTransformer, DimensionsCorrectAfterConstruction)
{
    BorderTransformer<BGRAPixel> sut{view, borders, color};
    EXPECT_EQ(sut.dimensions().upperLeftPoint.x, 0U);
    EXPECT_EQ(sut.dimensions().upperLeftPoint.y, 0U);
    EXPECT_EQ(sut.dimensions().width, baseDimensions.width + borders.left + borders.right);
    EXPECT_EQ(sut.dimensions().height, baseDimensions.height + borders.top + borders.bottom);
}

TEST_F(Transformation_BorderTransformer, CallsRelayedCorrectly)
{
    MockTransformer baseTransformer{};
    Borders const   b{0U, 0U, 2U, 1U};
    Rectangle const dim{4U, 3U};

    // called once construction and when reset or nextImage is successful
    EXPECT_CALL(baseTransformer, dimensions()).Times(3).WillRepeatedly(testing::Return(dim));
    EXPECT_CALL(baseTransformer, reset()).Times(2).WillOnce(testing::Return(true)).WillOnce(testing::Return(false));
    EXPECT_CALL(baseTransformer, nextImage()).Times(2).WillOnce(testing::Return(true)).WillOnce(testing::Return(false));
    BorderTransformer<BGRAPixel> sut{baseTransformer, b, color};
    EXPECT_TRUE(sut.reset());
    EXPECT_FALSE(sut.reset());
    EXPECT_TRUE(sut.nextImage());
    EXPECT_FALSE(sut.nextImage());
}

TEST_F(Transformation_BorderTransformer, CopyWorksOnTransformer)
{
    BorderTransformer<BGRAPixel> sut{};
    sut = BorderTransformer<BGRAPixel>(view, borders, color);
    EXPECT_EQ(sut.dimensions().width, baseDimensions.width + borders.left + borders.right);
    EXPECT_EQ(sut.dimensions().height, baseDimensions.height + borders.top + borders.bottom);
}

TEST_F(Transformation_BorderTransformer, TransformCallsCorrect)
{
    BorderTransformer<BGRAPixel> sut{view, borders, color};

    BGRAPixel  pixel{};
    auto const dimensions = sut.dimensions();

    auto myView = image.view();

    auto const topBorderPixelCount = dimensions.width * borders.top;
    for (auto i = 0U; i < topBorderPixelCount; ++i)
    {
        EXPECT_TRUE(sut.transform(pixel));
        ASSERT_EQ(pixel, color);
    }
    for (auto i = 0U; i < baseDimensions.height; ++i)
    {
        for (auto j = 0U; j < borders.left; ++j)
        {
            EXPECT_TRUE(sut.transform(pixel));
            ASSERT_EQ(pixel, color);
        }
        for (auto j = 0U; j < baseDimensions.width; ++j)
        {
            EXPECT_TRUE(sut.transform(pixel));
            ASSERT_EQ(pixel, *myView) << j << " " << i;
            ++myView;
        }
        for (auto j = 0U; j < borders.right; ++j)
        {
            EXPECT_TRUE(sut.transform(pixel));
            ASSERT_EQ(pixel, color);
        }
    }
    auto const bottomBorderPixelCount = dimensions.width * borders.bottom;
    for (auto i = 0U; i < bottomBorderPixelCount; ++i)
    {
        EXPECT_TRUE(sut.transform(pixel));
        ASSERT_EQ(pixel, color);
    }
    EXPECT_FALSE(sut.transform(pixel));
}

TEST_F(Transformation_BorderTransformer, TransformAndSkipCallsCorrect)
{
    BorderTransformer<BGRAPixel> sut{view, borders, color};

    BGRAPixel  pixel{};
    auto const dimensions = sut.dimensions();

    auto myView = image.view();

    auto skipTransform = false;

    auto const check = [&](BGRAPixel const &expectedColor, std::uint32_t const x, std::uint32_t const y) {
        if (skipTransform)
        {
            EXPECT_TRUE(sut.skip());
        }
        else
        {
            EXPECT_TRUE(sut.transform(pixel));
            ASSERT_EQ(pixel, expectedColor) << x << " " << y;
        }
    };

    auto const topBorderPixelCount = dimensions.width * borders.top;
    for (auto i = 0U; i < topBorderPixelCount; ++i)
    {
        check(color, i, 1000U);
    }
    for (auto i = 0U; i < baseDimensions.height; ++i)
    {
        for (auto j = 0U; j < borders.left; ++j)
        {
            check(color, j, i);
        }
        for (auto j = 0U; j < baseDimensions.width; ++j)
        {
            check(*myView, j, i);
            ++myView;
        }
        for (auto j = 0U; j < borders.right; ++j)
        {
            check(color, j, i);
        }
    }
    auto const bottomBorderPixelCount = dimensions.width * borders.bottom;
    for (auto i = 0U; i < bottomBorderPixelCount; ++i)
    {
        check(color, i, 2000U);
    }
    EXPECT_FALSE(sut.skip());
    EXPECT_FALSE(sut.transform(pixel));
}

} // namespace Terrahertz::UnitTests
