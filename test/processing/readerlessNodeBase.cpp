#include "THzImage/processing/readerlessNodeBase.hpp"

#include <gtest/gtest.h>
#include <vector>

namespace Terrahertz::UnitTests {

struct ProcessingReaderlessNodeBase : public testing::Test
{
    class TestReaderlessNode : public ImageProcessing::ReaderlessNodeBase<BGRAPixel>
    {
    public:
        TestReaderlessNode(size_t const bufferSize) noexcept : ReaderlessNodeBase(bufferSize) {}

        bool processResult{true};

        Rectangle nextDimensionsResult{2U, 2U};

        std::vector<size_t> countList{};

    private:
        Rectangle nextDimensions() const noexcept override { return nextDimensionsResult; }

        bool process(gsl::span<BGRAPixel> buffer, size_t const count) noexcept override
        {
            countList.push_back(count);
            if (processResult)
            {
                for (auto &pixel : buffer)
                {
                    pixel.blue = static_cast<std::uint8_t>(count);
                }
            }
            return processResult;
        }
    };

    using TestToCountResult = TestReaderlessNode::ToCountResult;
};

TEST_F(ProcessingReaderlessNodeBase, ConstructionAsExpected)
{
    Rectangle const defaultDimensions{};

    TestReaderlessNode sut{4U};
    EXPECT_EQ(sut.slots(), 4U);
    EXPECT_EQ(sut.count(), 0U);
    EXPECT_EQ(sut[0U].dimensions(), defaultDimensions);
    EXPECT_EQ(sut[4U].dimensions(), defaultDimensions);
}

TEST_F(ProcessingReaderlessNodeBase, CallingNextAdvancesCount)
{
    TestReaderlessNode sut{2U};
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut.count(), 1U);
    EXPECT_EQ(sut[0U].dimensions(), sut.nextDimensionsResult);
}

TEST_F(ProcessingReaderlessNodeBase, CallingNextReturnsFalseIfProcessReturnsFalse)
{
    TestReaderlessNode sut{2U};
    sut.processResult = false;
    EXPECT_FALSE(sut.next());
}

TEST_F(ProcessingReaderlessNodeBase, ProcessAccessesTheCorrectSlot)
{
    TestReaderlessNode sut{2U};
    EXPECT_TRUE(sut.next());
    EXPECT_TRUE(sut.next());
    auto const &image0 = sut[1U];
    EXPECT_EQ(image0[0U].blue, 1U);
    EXPECT_EQ(image0[1U].blue, 1U);
    EXPECT_EQ(image0[2U].blue, 1U);
    EXPECT_EQ(image0[3U].blue, 1U);
    auto const &image1 = sut[0U];
    EXPECT_EQ(image1[0U].blue, 2U);
    EXPECT_EQ(image1[1U].blue, 2U);
    EXPECT_EQ(image1[2U].blue, 2U);
    EXPECT_EQ(image1[3U].blue, 2U);
}

TEST_F(ProcessingReaderlessNodeBase, ToCountBasicBehavior)
{
    TestReaderlessNode sut{4U};
    EXPECT_EQ(sut.toCount(0U), TestToCountResult::NotUpdated);
    EXPECT_EQ(sut.count(), 0U);

    EXPECT_EQ(sut.toCount(4U), TestToCountResult::Updated);
    EXPECT_EQ(sut.count(), 4U);
    EXPECT_EQ(sut[0U][0U].blue, 4U);
    EXPECT_EQ(sut[1U][0U].blue, 3U);
    EXPECT_EQ(sut[2U][0U].blue, 2U);
    EXPECT_EQ(sut[3U][0U].blue, 1U);

    EXPECT_EQ(sut.toCount(2U), TestToCountResult::Ahead);
    EXPECT_EQ(sut.count(), 4U);

    sut.processResult = false;
    EXPECT_EQ(sut.toCount(6U), TestToCountResult::Failure);
    EXPECT_EQ(sut.count(), 4U);

    sut.processResult = true;
    EXPECT_EQ(sut.toCount(6U), TestToCountResult::Updated);
    EXPECT_EQ(sut.count(), 6U);
    EXPECT_EQ(sut[0U][0U].blue, 6U);
    EXPECT_EQ(sut[1U][0U].blue, 5U);
    EXPECT_EQ(sut[2U][0U].blue, 4U);
    EXPECT_EQ(sut[3U][0U].blue, 3U);
}

TEST_F(ProcessingReaderlessNodeBase, ToCountSkips)
{
    TestReaderlessNode sut{3U};
    EXPECT_EQ(sut.toCount(5U), TestToCountResult::Updated);
    EXPECT_EQ(sut.count(), 5U);

    EXPECT_EQ(sut.countList.size(), 3U);
    EXPECT_EQ(sut.countList[0U], 3U);
    EXPECT_EQ(sut.countList[1U], 4U);
    EXPECT_EQ(sut.countList[2U], 5U);
}

} // namespace Terrahertz::UnitTests
