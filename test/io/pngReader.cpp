#include "THzImage/io/pngReader.hpp"

#include "THzCommon/utility/spanhelpers.hpp"
#include "THzImage/common/image.hpp"
#include "THzImage/io/pngWriter.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_PNGReader : public testing::Test
{
    std::string filepath{"testRead.png"};
};

TEST_F(IO_PNGReader, NonExistingFile)
{
    PNG::Reader sut{"notHere.png"};
    EXPECT_FALSE(sut.fileTypeFits());
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
    EXPECT_TRUE(sut.imagePresent());
    EXPECT_EQ(sut.dimensions(), Rectangle{});
    EXPECT_TRUE(sut.fileTypeFits());
    EXPECT_TRUE(sut.init());
    EXPECT_EQ(sut.dimensions(), dimensions);

    std::array<BGRAPixel, 100U> loadedData{};
    EXPECT_TRUE(sut.read(toSpan<BGRAPixel>(loadedData)));
    sut.deinit();
    EXPECT_FALSE(sut.imagePresent());

    for (auto i = 0U; i < loadedData.size(); ++i)
    {
        ASSERT_EQ(loadedData[i], imageData[i]);
    }
}

} // namespace Terrahertz::UnitTests
