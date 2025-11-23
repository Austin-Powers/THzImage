#include "THzImage/processing/dataReductionNode.hpp"

#include "THzImage/io/pngWriter.hpp"
#include "THzImage/processing/fileInputNode.hpp"
#include "THzImage/processing/testInputNode.hpp"

#include <gtest/gtest.h>
#include <type_traits>
#include <vector>

#ifdef CURRENTLY_NOT_IMPLEMENTED

namespace Terrahertz::UnitTests {

struct ProcessingDataReductionNode : public testing::Test
{
    Rectangle const baseDimensions{24U, 31U};

    ImageProcessing::TestInputNode testNode{baseDimensions};

    MiniHSVImage replicateDataReduction(BGRAImage const &base, uint8_t const factor) noexcept
    {
        MiniHSVImage result{};

        auto const range   = base.dimensions().range(factor, factor, factor, factor);
        auto const tarDims = range.targetDimensions();
        if (result.setDimensions(Rectangle{tarDims.first, tarDims.second}))
        {
            for (auto const &zone : range)
            {
                BGRAPixel pixel{};
                for (auto const pos : zone)
                {
                    auto const &p = base[pos];
                    pixel.blue    = (pixel.blue < p.blue) ? p.blue : pixel.blue;
                    pixel.green   = (pixel.green < p.green) ? p.green : pixel.green;
                    pixel.red     = (pixel.red < p.red) ? p.red : pixel.red;
                }
                result[zone.targetPosition()] = pixel;
            }
        }
        return result;
    }
};

TEST_F(ProcessingDataReductionNode, CallingNextOfBaseNode)
{
    static_assert(
        std::is_same_v<ImageProcessing::TestInputNode::ToCountResult, ImageProcessing::FileInputNode::ToCountResult>,
        "uff");
    ImageProcessing::DataReductionNode sut{2U, true, testNode, std::uint8_t{2U}};

    // EXPECT_EQ(testNode.count(), 0U);
    // EXPECT_EQ(sut.count(), 0U);
    // EXPECT_TRUE(sut.next());
    // EXPECT_EQ(testNode.count(), 0U);
    // EXPECT_EQ(sut.count(), 1U);
    // EXPECT_TRUE(sut.next());
    // EXPECT_EQ(testNode.count(), 1U);
    // EXPECT_EQ(sut.count(), 2U);
}

TEST_F(ProcessingDataReductionNode, NotCallingNextOfBaseNode)
{
    ImageProcessing::DataReductionNode sut{2U, false, testNode, std::uint8_t{2U}};

    // EXPECT_EQ(testNode.count(), 0U);
    // EXPECT_EQ(sut.count(), 0U);
    // EXPECT_TRUE(sut.next());
    // EXPECT_EQ(testNode.count(), 0U);
    // EXPECT_EQ(sut.count(), 1U);
    // EXPECT_TRUE(sut.next());
    // EXPECT_EQ(testNode.count(), 0U);
    // EXPECT_EQ(sut.count(), 2U);
}

TEST_F(ProcessingDataReductionNode, EmptyImage)
{
    Rectangle const zeroDimensions{};

    ImageProcessing::TestInputNode     zeroTestNode{zeroDimensions};
    ImageProcessing::DataReductionNode sut{2U, true, zeroTestNode, std::uint8_t{2U}};

    EXPECT_FALSE(sut.next());
}

TEST_F(ProcessingDataReductionNode, DimensionsCalculatedCorrectly)
{
    // for (std::uint8_t i = 2U; i < 6U; ++i)
    //{
    //     Rectangle const expectedDimensions{baseDimensions.width / i, baseDimensions.height / i};
    //
    //     ImageProcessing::DataReductionNode sut{2U, true, testNode, i};
    //     EXPECT_TRUE(sut.next());
    //     EXPECT_EQ(sut[0U].dimensions(), expectedDimensions);
    // }
}

TEST_F(ProcessingDataReductionNode, ProcessingResultCorrect)
{
    // for (std::uint8_t i = 2U; i < 6U; ++i)
    // {
    //     ImageProcessing::DataReductionNode sut{2U, true, testNode, i};
    //     EXPECT_TRUE(sut.next());
    //
    //     MiniHSVImage const expectedResult = replicateDataReduction(testNode[0U], i);
    //     EXPECT_EQ(sut[0U], expectedResult);
    // }
}

} // namespace Terrahertz::UnitTests

#endif
