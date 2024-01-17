#include "THzImage/analysis/basicImageMetrics.hpp"

#include "THzCommon/math/rectangle.hpp"
#include "THzCommon/utility/fstreamhelpers.hpp"
#include "THzCommon/utility/spanhelpers.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <array>
#include <fstream>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct AnalysisBasicImageMetrics : public testing::Test
{
    std::string filepath{"test.txt"};
};

TEST_F(AnalysisBasicImageMetrics, DimensionsDoNotFitTheBuffer)
{
    BasicImageMetrics sut{filepath};
    EXPECT_TRUE(sut.init());
    Rectangle const dimensions{0, 0, 20U, 20U};

    std::array<BGRAPixel, 4U> imageData{};
    EXPECT_FALSE(sut.write(dimensions, toSpan<BGRAPixel const>(imageData)));
    sut.deinit();
}

} // namespace Terrahertz::UnitTests
