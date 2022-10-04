#include "THzImage/common/imageView.h"

#include "THzCommon/math/rectangle.h"
#include "THzImage/common/pixel.h"

#include <array>
#include <cstdint>
#include <gsl/gsl>
#include <gtest/gtest.h>
#include <iterator>

namespace Terrahertz::UnitTests {

struct Common_ImageView : public testing::Test
{
    using BGRAView = ImageView<BGRAPixel>;
    using HSVAView = ImageView<HSVAPixel>;

    static_assert(std::bidirectional_iterator<BGRAView>, "ImageView<BGRAPixel> is not a bidirectional_iterator");
    static_assert(std::bidirectional_iterator<HSVAView>, "ImageView<HSVAPixel> is not a bidirectional_iterator");
    static_assert(std::totally_ordered<BGRAView>, "ImageView<BGRAPixel> is not totally_ordered");
    static_assert(std::totally_ordered<HSVAView>, "ImageView<HSVAPixel> is not totally_ordered");
    // static_assert(std::sized_sentinel_for<BGRAView, BGRAView>,
    //               "ImageView<BGRAPixel> is not sized_sentinel_for BRGAView");
    // static_assert(std::sized_sentinel_for<HSVAView, HSVAView>,
    //               "ImageView<HSVAPixel> is not sized_sentinel_for HSVAView");

    static constexpr std::uint32_t const width{16};
    static constexpr std::uint32_t const height{9};

    Rectangle const dimensions{0, 0, width, height};
    Rectangle const region{2, 2, width - 4U, height - 4U};

    std::array<BGRAPixel, width * height> imageBuffer{};

    BGRAView sut{imageBuffer.data(), dimensions, region};
};

TEST_F(Common_ImageView, DefaultConstruction)
{
    BGRAView view{};
    EXPECT_EQ(view.basePointer(), nullptr);
    EXPECT_EQ(view.imageDimensions(), Rectangle{});
    EXPECT_EQ(view.region(), Rectangle{});
    EXPECT_EQ(view.currentPosition(), Point{});
    EXPECT_EQ(view.operator->(), nullptr);
}

TEST_F(Common_ImageView, Construction)
{
    EXPECT_EQ(sut.basePointer(), imageBuffer.data());
    EXPECT_EQ(sut.imageDimensions(), dimensions);
    EXPECT_EQ(sut.region(), region);
    EXPECT_EQ(sut.currentPosition(), region.upperLeftPoint);
    auto const pos = region.upperLeftPoint.x + (region.upperLeftPoint.y * dimensions.width);
    EXPECT_EQ(sut.operator->(), (imageBuffer.data() + pos));
    EXPECT_EQ(&(*sut), (imageBuffer.data() + pos));
}

TEST_F(Common_ImageView, ConstructionWidthUpperLeftPointInDimensions)
{
    Rectangle const dimensions{-2, 2, 20U, 20U};
    BGRAView const  view{imageBuffer.data(), dimensions};
    Rectangle const expectation{0, 0, 20U, 20U};
    EXPECT_EQ(view.imageDimensions(), expectation);
    EXPECT_EQ(view.region(), expectation);
}

TEST_F(Common_ImageView, ConstructionWithRegionOutsideImageDimensions)
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

TEST_F(Common_ImageView, SubView)
{
    for (auto i = -20; i < 20; ++i)
    {
        Rectangle const subRegion{i, i, 15U, 8U};

        auto const subView     = sut.subView(subRegion);
        auto const expectation = sut.region().intersection(subRegion);
        EXPECT_EQ(subView.region(), expectation);
    }
}

TEST_F(Common_ImageView, Increment)
{
    auto const compareViews = [](BGRAView const &a, BGRAView const &b) {
        ASSERT_EQ(a.currentPosition(), b.currentPosition());
        ASSERT_EQ(a.operator->(), b.operator->());
    };

    auto sut2 = sut;
    for (auto y = 0U; y < region.height; ++y)
    {
        for (auto x = 0U; x < region.width; ++x)
        {
            Point const expectedPosition{gsl::narrow_cast<std::int32_t>(region.upperLeftPoint.x + x),
                                         gsl::narrow_cast<std::int32_t>(region.upperLeftPoint.y + y)};
            ASSERT_EQ(sut.currentPosition(), expectedPosition);
            auto const pos = expectedPosition.x + (expectedPosition.y * dimensions.width);
            ASSERT_EQ(&(*sut), imageBuffer.data() + pos);

            auto const before = sut2;
            compareViews(sut2++, before);
            ASSERT_EQ(&(++sut), &sut);
            compareViews(sut, sut2);
        }
    }
}

TEST_F(Common_ImageView, Reset)
{
    auto const start = sut;
    ++sut;
    ++sut;
    ++sut;
    sut.reset();
    EXPECT_EQ(start.currentPosition(), sut.currentPosition());
    EXPECT_EQ(start.operator->(), sut.operator->());
}

TEST_F(Common_ImageView, ComparissonOperators)
{
    EXPECT_EQ(sut, sut);
    EXPECT_GE(sut, sut);
    EXPECT_LE(sut, sut);

    auto const sutCopy = sut;
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

TEST_F(Common_ImageView, EndReturnsCorrectResult)
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

TEST_F(Common_ImageView, Decrement)
{
    auto const compareViews = [](BGRAView const &a, BGRAView const &b) {
        ASSERT_EQ(a.currentPosition(), b.currentPosition());
        ASSERT_EQ(a.operator->(), b.operator->());
    };

    auto postDec = sut.end();
    auto preDec  = sut.end();
    for (auto y = region.height - 1U; y < region.height; --y)
    {
        for (auto x = region.width - 1U; x < region.width; --x)
        {
            auto const before = postDec;
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

} // namespace Terrahertz::UnitTests
