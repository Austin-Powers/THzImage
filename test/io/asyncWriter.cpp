#include "THzImage/io/asyncWriter.hpp"

#include "THzImage/common/iImageWriter.hpp"
#include "THzImage/common/image.hpp"
#include "THzImage/common/pixel.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <thread>

namespace Terrahertz::UnitTests {

struct IO_AsyncWriter : public testing::Test
{
    struct MockWriter : public IImageWriter<BGRAPixel>
    {
        bool init() noexcept override
        {
            initCalled = true;
            return true;
        }

        bool write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept override
        {
            std::this_thread::sleep_for(std::chrono::milliseconds{16U});
            return true;
        }

        void deinit() noexcept override { writeFinished = true; }

        bool initCalled{};

        bool writeFinished{};
    };

    MockWriter writer{};
};

TEST_F(IO_AsyncWriter, GeneralOperation)
{
    std::chrono::milliseconds interval{4U};
    BGRAImage                 image{};
    ASSERT_TRUE(image.setDimensions(Rectangle{2U, 2U}));

    // check basics
    AsyncWriter sut{writer};
    EXPECT_TRUE(sut.write(image));
    std::this_thread::sleep_for(interval);
    EXPECT_TRUE(writer.initCalled);
    EXPECT_FALSE(writer.writeFinished);

    // check if the writer rejects new images till the last one is finished
    EXPECT_FALSE(sut.write(image));
    while (!writer.writeFinished)
    {
        std::this_thread::sleep_for(interval);
    }

    // check if the reader work normally after finishing the writing
    writer.initCalled    = false;
    writer.writeFinished = false;

    EXPECT_TRUE(sut.write(image));
    std::this_thread::sleep_for(interval);
    EXPECT_TRUE(writer.initCalled);
    EXPECT_FALSE(writer.writeFinished);

    // check again if the writer rejects new images till the last one is finished
    EXPECT_FALSE(sut.write(image));
}

} // namespace Terrahertz::UnitTests
