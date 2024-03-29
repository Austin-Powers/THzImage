#include "THzImage/common/imageView.hpp"

#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/image.hpp"
#include "THzImage/common/pixel.hpp"

#include <array>
#include <cstdint>
#include <gsl/gsl>
#include <gtest/gtest.h>
#include <iterator>

namespace Terrahertz::UnitTests {

struct CommonImageView : public testing::Test
{
    using BGRAView = ImageView<BGRAPixel>;
    using HSVAView = ImageView<HSVAPixel>;

    static_assert(std::bidirectional_iterator<BGRAView>, "ImageView<BGRAPixel> is not a bidirectional_iterator");
    static_assert(std::bidirectional_iterator<HSVAView>, "ImageView<HSVAPixel> is not a bidirectional_iterator");
    static_assert(std::totally_ordered<BGRAView>, "ImageView<BGRAPixel> is not totally_ordered");
    static_assert(std::totally_ordered<HSVAView>, "ImageView<HSVAPixel> is not totally_ordered");

    static constexpr std::uint32_t const width{16};
    static constexpr std::uint32_t const height{9};

    Rectangle const dimensions{0, 0, width, height};
    Rectangle const region{2, 2, width - 4U, height - 4U};

    std::array<BGRAPixel, width * height> imageBuffer{};

    BGRAView sut{imageBuffer.data(), dimensions, region};
};

TEST_F(CommonImageView, DefaultConstruction)
{
    BGRAView view{};
    EXPECT_EQ(view.basePointer(), nullptr);
    EXPECT_EQ(view.imageDimensions(), Rectangle{});
    EXPECT_EQ(view.region(), Rectangle{});
    EXPECT_EQ(view.currentPosition(), Point{});
    EXPECT_EQ(view.operator->(), nullptr);
}

TEST_F(CommonImageView, Construction)
{
    EXPECT_EQ(sut.basePointer(), imageBuffer.data());
    EXPECT_EQ(sut.imageDimensions(), dimensions);
    EXPECT_EQ(sut.region(), region);
    EXPECT_EQ(sut.currentPosition(), region.upperLeftPoint);
    auto const pos = region.upperLeftPoint.x + (region.upperLeftPoint.y * dimensions.width);
    EXPECT_EQ(sut.operator->(), (imageBuffer.data() + pos));
    EXPECT_EQ(&(*sut), (imageBuffer.data() + pos));
}

TEST_F(CommonImageView, ConstructionWidthUpperLeftPointInDimensions)
{
    Rectangle const dimensions{-2, 2, 20U, 20U};
    BGRAView const  view{imageBuffer.data(), dimensions};
    Rectangle const expectation{0, 0, 20U, 20U};
    EXPECT_EQ(view.imageDimensions(), expectation);
    EXPECT_EQ(view.region(), expectation);
}

TEST_F(CommonImageView, ConstructionWithRegionOutsideImageDimensions)
{
    Rectangle const dimensions{-2, 2, 20U, 20U};
    for (auto i = -23; i < 20; ++i)
    {
        Rectangle const region{i, i, 22U, 22U};
        BGRAView const  view{imageBuffer.data(), dimensions, region};
        Rectangle const expectation{0, 0, 20U, 20U};
        EXPECT_EQ(view.imageDimensions(), expectation);
        EXPECT_EQ(view.region(), expectation.intersection(region));
    }
}

TEST_F(CommonImageView, SubView)
{
    for (auto i = -20; i < 20; ++i)
    {
        Rectangle const subRegion{i, i, 15U, 8U};

        auto const subView     = sut.subView(subRegion);
        auto const expectation = sut.region().intersection(subRegion);
        EXPECT_EQ(subView.region(), expectation);
    }
}

TEST_F(CommonImageView, Increment)
{
    auto const compareViews = [](BGRAView const &a, BGRAView const &b) {
        ASSERT_EQ(a.currentPosition(), b.currentPosition());
        ASSERT_EQ(a.operator->(), b.operator->());
    };

    BGRAView sut2 = sut;
    for (auto y = 0U; y < region.height; ++y)
    {
        for (auto x = 0U; x < region.width; ++x)
        {
            Point const expectedPosition{gsl::narrow_cast<std::int32_t>(region.upperLeftPoint.x + x),
                                         gsl::narrow_cast<std::int32_t>(region.upperLeftPoint.y + y)};
            ASSERT_EQ(sut.currentPosition(), expectedPosition);
            auto const pos = expectedPosition.x + (expectedPosition.y * dimensions.width);
            ASSERT_EQ(&(*sut), imageBuffer.data() + pos);

            BGRAView const before = sut2;
            compareViews(sut2++, before);
            ASSERT_EQ(&(++sut), &sut);
            compareViews(sut, sut2);
        }
    }
}

TEST_F(CommonImageView, Reset)
{
    BGRAView const start = sut;
    ++sut;
    ++sut;
    ++sut;
    EXPECT_TRUE(sut.reset());
    EXPECT_EQ(start.currentPosition(), sut.currentPosition());
    EXPECT_EQ(start.operator->(), sut.operator->());
}

TEST_F(CommonImageView, ComparissonOperators)
{
    EXPECT_EQ(sut, sut);
    EXPECT_GE(sut, sut);
    EXPECT_LE(sut, sut);

    BGRAView const sutCopy = sut;
    EXPECT_EQ(sutCopy, sut);
    EXPECT_GE(sutCopy, sut);
    EXPECT_LE(sutCopy, sut);

    ++sut;
    EXPECT_NE(sutCopy, sut);
    EXPECT_LE(sutCopy, sut);
    EXPECT_LT(sutCopy, sut);
    EXPECT_GT(sut, sutCopy);
    EXPECT_GE(sut, sutCopy);
    EXPECT_NE(sut, sutCopy);
}

TEST_F(CommonImageView, EndReturnsCorrectResult)
{
    auto       count = 0U;
    auto const end   = sut.end();
    while (sut != end)
    {
        ++count;
        ++sut;

        if (count > (sut.region().area() * 2U))
        {
            // abort if we ran clearly beyond the region
            FAIL();
        }
    }
    EXPECT_EQ(count, sut.region().area());
    EXPECT_EQ(sut.currentPosition(), end.currentPosition());
}

TEST_F(CommonImageView, Decrement)
{
    auto const compareViews = [](BGRAView const &a, BGRAView const &b) {
        ASSERT_EQ(a.currentPosition(), b.currentPosition());
        ASSERT_EQ(a.operator->(), b.operator->());
    };

    auto postDec = sut.end();
    auto preDec  = sut.end();
    for (auto y = region.height - 1U; y != std::numeric_limits<std::uint32_t>::max(); --y)
    {
        for (auto x = region.width - 1U; x != std::numeric_limits<std::uint32_t>::max(); --x)
        {
            BGRAView const before = postDec;
            compareViews(postDec--, before);
            ASSERT_EQ(&(--preDec), &preDec);
            compareViews(preDec, postDec);

            Point const expectedPosition{gsl::narrow_cast<std::int32_t>(region.upperLeftPoint.x + x),
                                         gsl::narrow_cast<std::int32_t>(region.upperLeftPoint.y + y)};
            ASSERT_EQ(preDec.currentPosition(), expectedPosition);
            auto const pos = expectedPosition.x + (expectedPosition.y * dimensions.width);
            ASSERT_EQ(&(*preDec), imageBuffer.data() + pos);
        }
    }
}

TEST_F(CommonImageView, ImageTransformerImplementation)
{
    Rectangle const expectedDimensions{0, 0, region.width, region.height};
    EXPECT_EQ(sut.dimensions(), expectedDimensions);
    auto count = 0U;
    sut->blue  = count;
    while (sut.skip())
    {
        ++count;
        sut->blue = count;
        if (count > expectedDimensions.area() * 2U)
        {
            FAIL();
        }
    }
    EXPECT_EQ(count, expectedDimensions.area());

    count = 0U;
    sut.reset();
    for (auto i = 0U; i < 61U; ++i)
    {
        BGRAPixel expectation{};
        expectation.blue = i;
        BGRAPixel  pixel{};
        auto const res = sut.transform(pixel);
        if (i < 60U)
        {
            EXPECT_TRUE(res);
            EXPECT_EQ(expectation, pixel);
        }
        else
        {
            EXPECT_FALSE(res);
        }
    }
}

TEST_F(CommonImageView, ForeachLoopCompatibility)
{
    auto count = 0U;
    for (auto &pixel : sut)
    {
        pixel.blue = count;
        ++count;
    }
    EXPECT_EQ(count, region.area());

    count = 0U;
    for (auto y = 0U; y < dimensions.height; ++y)
    {
        for (auto x = 0U; x < dimensions.width; ++x)
        {
            if ((2U > y) || (y >= (dimensions.height - 2U)) || (2U > x) || (x >= (dimensions.width - 2U)))
            {
                EXPECT_EQ(imageBuffer[x + (y * dimensions.width)].blue, 0U);
            }
            else
            {
                EXPECT_EQ(imageBuffer[x + (y * dimensions.width)].blue, count);
                ++count;
            }
        }
    }
}

TEST_F(CommonImageView, DefaultConstructedViewDoesNotThrowIfUsedByImage)
{
    BGRAView  view{};
    BGRAImage image{};
    EXPECT_FALSE(image.executeAndIngest(view));
}

} // namespace Terrahertz::UnitTests
