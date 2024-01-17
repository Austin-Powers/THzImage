#include "THzImage/io/timedReader.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <chrono>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IOTimedReader : public testing::Test
{
    using BaseReader = TestImageGenerator;
    using TestReader = TimedReader<BaseReader>;
    using millisecs  = std::chrono::milliseconds;
};

TEST_F(IOTimedReader, NewlyConstructedInstanceCorrect)
{
    Rectangle const dimensions{10U, 10U};
    millisecs const interval{10U};

    BaseReader expectedInnerReader{dimensions};
    TestReader sut{interval, dimensions};

    EXPECT_EQ(expectedInnerReader.dimensions(), sut.wrappedReader().dimensions());
    EXPECT_EQ(sut.interval(), interval);
    EXPECT_EQ(sut.info().missedTimeSlots, 0U);
    EXPECT_EQ(sut.info().totalTimeSlots, 0U);
}

TEST_F(IOTimedReader, ConstructionWithZeroInterval)
{
    TestReader sut{millisecs{0U}, Rectangle{2U, 2U}};
    EXPECT_EQ(sut.interval(), millisecs{1000U});
}

TEST_F(IOTimedReader, UpdatingInterval)
{
    millisecs const interval{10U};

    TestReader sut{interval, Rectangle{2U, 2U}};
    EXPECT_FALSE(sut.setInterval(millisecs{0U}));
    EXPECT_EQ(sut.interval(), interval);

    millisecs newInterval{100U};
    EXPECT_TRUE(sut.setInterval(newInterval));
    EXPECT_EQ(sut.interval(), newInterval);
    EXPECT_EQ(sut.info().missedTimeSlots, 0U);
    EXPECT_EQ(sut.info().totalTimeSlots, 0U);
}

// As long as these tests do not fail in release everything is fine
#ifdef NDEBUG

TEST_F(IOTimedReader, DelayReadingAsExpected)
{
    millisecs const    interval{16U};
    Rectangle const    dimensions{2U, 2U};
    BGRAImage          referenceImage{};
    TestImageGenerator generator{dimensions};
    EXPECT_TRUE(referenceImage.read(generator));

    BGRAImage  image{};
    TestReader sut{interval, dimensions};
    auto const startTime = std::chrono::steady_clock::now();

    auto const targetCount = 12LL;
    auto       durations   = 0LL;
    for (auto i = 1LL; i <= targetCount; ++i)
    {
        EXPECT_TRUE(image.read(sut));
        auto const readTime = std::chrono::steady_clock::now();
        auto const duration = std::chrono::duration_cast<millisecs>(readTime - startTime);
        durations += (duration.count() / i);

        ASSERT_EQ(referenceImage.dimensions(), image.dimensions());
        for (auto const idx : image.dimensions().range())
        {
            EXPECT_EQ(image[idx], referenceImage[idx]);
        }
    }
    durations /= targetCount;
    EXPECT_LE(durations, interval.count() + 4);
    EXPECT_GE(durations, interval.count() - 4);
    EXPECT_EQ(sut.info().missedTimeSlots, 0U);
    EXPECT_EQ(sut.info().totalTimeSlots, targetCount);
}

TEST_F(IOTimedReader, ReaderDelaysToFixTimePoints)
{
    millisecs const interval{20U};
    millisecs const sleepInterval{interval.count() + 2U};
    Rectangle const dimensions{2U, 2U};

    BGRAImage  image{};
    TestReader sut{interval, dimensions};
    auto const startTime = std::chrono::steady_clock::now();

    auto const targetCount = 12LL;
    auto       durations   = 0LL;
    for (auto i = 1LL; i <= targetCount; ++i)
    {
        std::this_thread::sleep_for(sleepInterval);
        EXPECT_TRUE(image.read(sut));
        auto const readTime = std::chrono::steady_clock::now();
        auto const duration = std::chrono::duration_cast<millisecs>(readTime - startTime);
        durations += (duration.count() / i);
    }
    durations /= targetCount;
    EXPECT_LE(durations, (interval.count() * 2) + 4);
    EXPECT_GE(durations, (interval.count() * 2) - 4);
    EXPECT_EQ(sut.info().missedTimeSlots, targetCount);
    EXPECT_EQ(sut.info().totalTimeSlots, targetCount * 2U);

    // as these test are rather time consuming, we will test here if setInterval resets the info
    EXPECT_FALSE(sut.setInterval(millisecs{0U}));
    EXPECT_EQ(sut.info().missedTimeSlots, targetCount);
    EXPECT_EQ(sut.info().totalTimeSlots, targetCount * 2U);

    EXPECT_TRUE(sut.setInterval(millisecs{20U}));
    EXPECT_EQ(sut.info().missedTimeSlots, 0U);
    EXPECT_EQ(sut.info().totalTimeSlots, 0U);
}
#endif // !NDEBUG

} // namespace Terrahertz::UnitTests
