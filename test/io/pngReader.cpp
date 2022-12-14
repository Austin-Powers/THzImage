#include "THzImage/io/pngReader.h"

#include "THzCommon/utility/spanhelpers.h"
#include "THzImage/common/image.h"
#include "THzImage/io/pngWriter.h"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_PNGReader : public testing::Test
{
    std::string filepath{"testRead.png"};
};

TEST_F(IO_PNGReader, NonExistingFile)
{
    PNG::Reader sut{"notHere.png"};
    EXPECT_FALSE(sut.init());
}

TEST_F(IO_PNGReader, ConstructionCorrect)
{
    std::array<BGRAPixel, 100U> imageData{};

    std::uint8_t index = 1U;
    for (auto &pixel : imageData)
    {
        pixel.red   = index;
        pixel.green = index;
        pixel.blue  = index;
        ++index;
    }

    Rectangle const dimensions{10U, 10U};

    PNG::Writer writer{filepath};
    writer.init();
    writer.write(dimensions, toSpan<BGRAPixel const>(imageData));
    writer.deinit();

    PNG::Reader sut{filepath};
    EXPECT_FALSE(sut.multipleImages());
    EXPECT_EQ(sut.dimensions(), Rectangle{});
    EXPECT_TRUE(sut.init());
    EXPECT_EQ(sut.dimensions(), dimensions);

    std::array<BGRAPixel, 100U> loadedData{};
    EXPECT_TRUE(sut.read(toSpan<BGRAPixel>(loadedData)));
    sut.deinit();

    for (auto i = 0U; i < loadedData.size(); ++i)
    {
        ASSERT_EQ(loadedData[i], imageData[i]);
    }
}

} // namespace Terrahertz::UnitTests
