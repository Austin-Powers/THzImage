#include "THzImage/io/pngWriter.hpp"

#include "THzCommon/utility/fstreamhelpers.hpp"
#include "THzImage/common/image.hpp"

#include <array>
#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_PNGWriter : public testing::Test
{
    std::string filepath{"testWrite.png"};

    void TearDown() noexcept override { std::remove(filepath.c_str()); }
};

TEST_F(IO_PNGWriter, DimensionsDoNotFitTheBufferSize)
{
    PNG::Writer sut{filepath};
    EXPECT_TRUE(sut.init());
    Rectangle const dimensions{0, 0, 20U, 20U};

    std::array<BGRAPixel, 4U> imageData{};
    EXPECT_FALSE(sut.write(dimensions, toSpan<BGRAPixel const>(imageData)));
    sut.deinit();
}

TEST_F(IO_PNGWriter, WritingCreatesFile)
{
    PNG::Writer sut{filepath};
    EXPECT_TRUE(sut.init());
    Rectangle const dimensions{0, 0, 10U, 10U};

    std::array<BGRAPixel, 100U> imageData{};
    EXPECT_TRUE(sut.write(dimensions, toSpan<BGRAPixel const>(imageData)));
    sut.deinit();

    std::ifstream stream{filepath, std::ios::binary};

    std::array<std::uint8_t, 4U> magic{};
    EXPECT_EQ(readFromStream(stream, magic), 4U);
    EXPECT_EQ(magic[0U], 137U);
    EXPECT_EQ(magic[1U], 'P');
    EXPECT_EQ(magic[2U], 'N');
    EXPECT_EQ(magic[3U], 'G');
}

// if data is written correctly will be tested along with the Reader
// additional tests are omitted due to libpng being tested by its devs anyway

} // namespace Terrahertz::UnitTests
