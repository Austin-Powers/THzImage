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
    using HSVAView = ImageView<HSVAPixel>;

    static constexpr std::uint32_t const width{16};
    static constexpr std::uint32_t const height{9};

    Rectangle const dimensions{0, 0, width, height};
    Rectangle const region{2, 2, width - 4U, height - 4U};

    std::array<BGRAPixel, width * height> imageBuffer{};

    ImageView<BGRAPixel> sut{imageBuffer.data(), dimensions, region};
};

TEST_F(Common_ImageView, DefaultConstruction)
{
    ImageView<BGRAPixel> view{};
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

} // namespace Terrahertz::UnitTests
