#include "THzImage/common/imageView.h"

#include "THzCommon/math/rectangle.h"
#include "THzImage/common/pixel.h"

#include <array>
#include <cstdint>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct Common_ImageView : public testing::Test
{
    using BGRAView = ImageView<BGRAPixel>;

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
}

TEST_F(Common_ImageView, Construction)
{
    EXPECT_EQ(sut.basePointer(), imageBuffer.data());
    EXPECT_EQ(sut.imageDimensions(), dimensions);
    EXPECT_EQ(sut.region(), region);
    EXPECT_EQ(sut.currentPosition(), Point{});
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
    for (auto i = -23; i < 40; ++i)
    {
        Rectangle const region{i, i, 22U, 22U};
        BGRAView const  view{imageBuffer.data(), dimensions, region};
        Rectangle const expectation{0, 0, 20U, 20U};
        EXPECT_EQ(view.imageDimensions(), expectation);
        EXPECT_EQ(view.region(), expectation.intersection(region));
    }
}

} // namespace Terrahertz::UnitTests
