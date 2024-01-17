#include "THzImage/transformation/nullTransformer.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct TransformationNullTransformer : public testing::Test
{};

TEST_F(TransformationNullTransformer, ReturnValuesAsExpected)
{
    auto &instance = NullTransformer<BGRAPixel>::instance();
    EXPECT_EQ(instance.dimensions(), Rectangle{});
    BGRAPixel const expectedPixel{0x34U, 0x11U, 0xBAU, 0xFEU};
    BGRAPixel       pixel{expectedPixel};
    EXPECT_FALSE(instance.transform(pixel));
    EXPECT_EQ(pixel, expectedPixel);
    EXPECT_FALSE(instance.skip());
    EXPECT_FALSE(instance.reset());
    EXPECT_FALSE(instance.nextImage());
}

} // namespace Terrahertz::UnitTests
