#ifdef _WIN32

#include "THzImage/io/screenReader.h"

#include "THzImage/common/image.h"

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

TEST_F(IO_ScreenReader, DimensionsReturnsValueGivenToSetArea)
{
    Screen::Reader            sut{};
    std::array<Rectangle, 3U> testValues{
        Rectangle(0, 0, 24U, 32U), Rectangle(16, 24, 32U, 48U), Screen::Reader::getScreenDimensions()};

    for (auto const &value : testValues)
    {
        EXPECT_TRUE(sut.setArea(value));
        EXPECT_EQ(sut.dimensions(), value);
    }
}

TEST_F(IO_ScreenReader, SetAreaRejectsValuesOutsideTheScreen)
{
    Screen::Reader sut{};
    auto const     screenDimensions = Screen::Reader::getScreenDimensions();
    for (auto i = 0U; i < 4U; ++i)
    {
        auto dim = screenDimensions;
        switch (i)
        {
        case 0:
            dim.upperLeftPoint.x -= 4U;
            break;
        case 1:
            dim.upperLeftPoint.y -= 4U;
            break;
        case 2:
            dim.width += 4U;
            break;
        default:
            dim.height += 4U;
            break;
        }
        EXPECT_FALSE(sut.setArea(dim));
        EXPECT_EQ(sut.dimensions(), screenDimensions);
    }
}

TEST_F(IO_ScreenReader, ConstructionUsingAreaOutsideScreenDefaultsToScreenDimensions)
{
    auto const screenDimensions = Screen::Reader::getScreenDimensions();
    for (auto i = 0U; i < 4U; ++i)
    {
        auto dim = screenDimensions;
        switch (i)
        {
        case 0:
            dim.upperLeftPoint.x -= 4U;
            break;
        case 1:
            dim.upperLeftPoint.y -= 4U;
            break;
        case 2:
            dim.width += 4U;
            break;
        default:
            dim.height += 4U;
            break;
        }
        Screen::Reader sut{dim};
        EXPECT_EQ(sut.dimensions(), screenDimensions);
    }
}

TEST_F(IO_ScreenReader, TakeScreenshot)
{
    BGRAPixel const startColor{0xFFU, 0x00F, 0xFFU};
    Rectangle const dimensions{100, 100, 32U, 32U};
    BGRAImage       image{};
    EXPECT_TRUE(image.setDimensions(dimensions));
    for (auto &pixel : image.view())
    {
        pixel = startColor;
    }

    Screen::Reader sut{dimensions};
    EXPECT_TRUE(image.read(&sut));

    // we cannot expect what is on the screen exactly, so we check for changes
    auto changedPixels = 0U;
    for (auto &pixel : image.view())
    {
        if (pixel != startColor)
        {
            ++changedPixels;
        }
    }
    EXPECT_GE(changedPixels, (dimensions.area() - dimensions.width));
}

TEST_F(IO_ScreenReader, ChangeAreaBetweenScreenshots)
{
    Rectangle const dimensions{100, 100, 32U, 32U};
    BGRAImage       image0{};
    BGRAImage       image1{};

    Screen::Reader sut{dimensions};
    EXPECT_TRUE(image0.read(&sut));
    EXPECT_TRUE(sut.setArea(Rectangle{200, 200, dimensions.width, dimensions.height}));
    EXPECT_TRUE(image1.read(&sut));

    // we cannot expect what is on the screen exactly, so we if the areas differ
    auto changedPixels = 0U;
    for (auto i = 0U; i < dimensions.area(); ++i)
    {
        if (image0[i] != image1[i])
        {
            ++changedPixels;
        }
    }
    EXPECT_GE(changedPixels, dimensions.width);
}

} // namespace Terrahertz::UnitTests

#endif // !_WIN32
