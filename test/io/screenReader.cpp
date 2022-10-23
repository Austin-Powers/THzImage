#ifdef _WIN32

#include "THzImage/io/screenReader.h"

#include <Windows.h>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_ScreenReader : public testing::Test
{};

TEST_F(IO_ScreenReader, GetScreenDimensions)
{
    auto const screenDimensions = Screen::Reader::getScreenDimensions();
    EXPECT_EQ(screenDimensions.width, GetSystemMetrics(SM_CXSCREEN));
    EXPECT_EQ(screenDimensions.height, GetSystemMetrics(SM_CYSCREEN));
}

TEST_F(IO_ScreenReader, DefaultConstructionCorrect)
{
    Screen::Reader sut{};
    auto const     expectedDimensions = Screen::Reader::getScreenDimensions();
    EXPECT_TRUE(sut.multipleImages());
    EXPECT_EQ(sut.dimensions(), expectedDimensions);
}

} // namespace Terrahertz::UnitTests

#endif // !_WIN32
