#include "THzImage/io/imageSeriesReader.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/io/bmpReader.hpp"
#include "THzImage/io/pngReader.hpp"
#include "THzImage/io/qoiReader.hpp"
#include "THzImage/io/qoiWriter.hpp"

#include <cstdint>
#include <fstream>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_ImageSeriesReader : public testing::Test
{
    struct ImageInfo
    {
        std::string   name{};
        std::uint32_t width{};
        std::uint32_t height{};
        std::uint8_t  startValue{};
    };
    std::array<ImageInfo const, 4U> infos{ImageInfo{".\\seriesTest0.qoi", 3U, 3U, 0U},
                                          ImageInfo{".\\seriesTest1.qoi", 3U, 4U, 2U},
                                          ImageInfo{".\\seriesTest2.qoi", 7U, 2U, 4U},
                                          ImageInfo{".\\seriesTest3.qoi", 5U, 3U, 7U}};

    void createTestImages() noexcept
    {
        auto const setupImage = [](ImageInfo const &info) {
            BGRAImage image{};
            EXPECT_TRUE(image.setDimensions(Rectangle{info.width, info.height}));

            auto value = info.startValue;
            for (auto const idx : image.dimensions().range())
            {
                image[idx].blue  = value;
                image[idx].green = value;
                image[idx].red   = value;
                ++value;
            }
            QOI::Writer writer{info.name};
            EXPECT_TRUE(image.write(&writer));
        };

        setupImage(infos[0U]);
        setupImage(infos[1U]);
        setupImage(infos[2U]);
        setupImage(infos[3U]);
    }
};

TEST_F(IO_ImageSeriesReader, ConstructionPossible)
{
    // mainly checks if the available file readers conform to the concepts
    ImageSeries::Reader<BMP::Reader> testBMP{"."};
    ImageSeries::Reader<PNG::Reader> testPNG{"."};
    ImageSeries::Reader<QOI::Reader> testQOI{"."};
}

TEST_F(IO_ImageSeriesReader, ConstructionUsingNonExistingDirectory)
{
    ImageSeries::Reader<QOI::Reader> sut{"/folder"};
    EXPECT_FALSE(sut.imagePresent());
    EXPECT_TRUE(sut.currentFilepath().empty());
}

TEST_F(IO_ImageSeriesReader, ConstructionUsingPathToAFile)
{
    auto const filepath = "test.txt";

    // create test file
    std::ofstream file{filepath};
    file << "test text for testing" << std::endl;
    file.close();
    ImageSeries::Reader<QOI::Reader> sut{filepath};
    EXPECT_FALSE(sut.imagePresent());
    EXPECT_TRUE(sut.currentFilepath().empty());
}

TEST_F(IO_ImageSeriesReader, OperationCorrect)
{
    createTestImages();
    ImageSeries::Reader<QOI::Reader> sut{"."};

    BGRAImage image{};

    auto images = 0U;
    for (auto i = 0U; (i < 16U) && sut.imagePresent(); ++i)
    {
        auto const filepath = sut.currentFilepath();
        ASSERT_TRUE(image.read(&sut));
        for (auto const &info : infos)
        {
            if (std::filesystem::path{info.name} == filepath)
            {
                ASSERT_EQ(image.dimensions().width, info.width);
                ASSERT_EQ(image.dimensions().height, info.height);
                for (auto const idx : image.dimensions().range())
                {
                    auto const &pxl   = image[idx];
                    auto const  value = info.startValue + idx;
                    ASSERT_EQ(pxl.blue, value);
                    ASSERT_EQ(pxl.green, value);
                    ASSERT_EQ(pxl.red, value);
                }
                ++images;
            }
        }
    }
    EXPECT_EQ(images, infos.size());
}

} // namespace Terrahertz::UnitTests
