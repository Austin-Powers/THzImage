#include "THzImage/io/imageSeriesWriter.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/io/bmpWriter.hpp"
#include "THzImage/io/pngWriter.hpp"
#include "THzImage/io/qoiReader.hpp"
#include "THzImage/io/qoiWriter.hpp"

#include <cstdint>
#include <fstream>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IOImageSeriesWriter : public testing::Test
{};

TEST_F(IOImageSeriesWriter, CreationPossible)
{
    // mainly checks if the available file writers conform to the concepts
    EXPECT_TRUE(ImageSeries::Writer<BMP::Writer>::createWriter("./?.bmp"));
    EXPECT_TRUE(ImageSeries::Writer<PNG::Writer>::createWriter("./?.png"));
    EXPECT_TRUE(ImageSeries::Writer<QOI::Writer>::createWriter("./?.qoi"));
}

TEST_F(IOImageSeriesWriter, CreationFailsIfFilepathDoesNotFit)
{
    auto writer = ImageSeries::Writer<QOI::Writer>::createWriter("./test.qoi");
    EXPECT_FALSE(writer);
    writer = ImageSeries::Writer<QOI::Writer>::createWriter("./???.qoi");
    EXPECT_FALSE(writer);
    writer = ImageSeries::Writer<QOI::Writer>::createWriter("./?_?_?.qoi");
    EXPECT_FALSE(writer);
}

TEST_F(IOImageSeriesWriter, CreationSucceedsIfFilepathDoesFit)
{
    auto writer = ImageSeries::Writer<QOI::Writer>::createWriter("./?.qoi");
    EXPECT_TRUE(writer);
    writer = ImageSeries::Writer<QOI::Writer>::createWriter("./?/test.qoi");
    EXPECT_TRUE(writer);
}

TEST_F(IOImageSeriesWriter, OverallOperation)
{
    auto writer = ImageSeries::Writer<QOI::Writer>::createWriter("./?.qoi");
    ASSERT_TRUE(writer);

    BGRAImage  image{};
    auto const checkImage = [&](char const *const filepath) noexcept {
        BGRAImage   loadedImage{};
        QOI::Reader reader{filepath};
        ASSERT_TRUE(loadedImage.read(reader));
        ASSERT_EQ(image.dimensions(), loadedImage.dimensions());
        for (auto const idx : image.dimensions().range())
        {
            EXPECT_EQ(image[idx], loadedImage[idx]);
        }
    };

    EXPECT_TRUE(image.setDimensions(Rectangle{1U, 1U}));
    image[0U] = BGRAPixel{10U, 10U, 10U};
    EXPECT_TRUE(image.write(&(*writer)));
    checkImage("./000000.qoi");
    EXPECT_TRUE(image.setDimensions(Rectangle{2U, 3U}));
    image[2U] = BGRAPixel{10U, 10U, 10U};
    image[4U] = BGRAPixel{10U, 10U, 10U};
    EXPECT_TRUE(image.write(&(*writer)));
    checkImage("./000001.qoi");
    EXPECT_TRUE(image.setDimensions(Rectangle{3U, 1U}));
    image[0U] = BGRAPixel{10U, 10U, 10U};
    image[1U] = BGRAPixel{12U, 12U, 12U};
    EXPECT_TRUE(image.write(&(*writer)));
    checkImage("./000002.qoi");
}

TEST_F(IOImageSeriesWriter, GivingIncrementsOfZeroFailsCreation)
{
    EXPECT_FALSE(ImageSeries::Writer<QOI::Writer>::createWriter("./?.qoi", 0U, 0U));
}

TEST_F(IOImageSeriesWriter, StartNumberAndIncrementNotDefaultValues)
{
    auto writer = ImageSeries::Writer<QOI::Writer>::createWriter("./32_?.qoi", 3U, 2U);
    ASSERT_TRUE(writer);

    BGRAImage  image{};
    auto const checkImage = [&](char const *const filepath) noexcept {
        BGRAImage   loadedImage{};
        QOI::Reader reader{filepath};
        ASSERT_TRUE(loadedImage.read(reader));
        ASSERT_EQ(image.dimensions(), loadedImage.dimensions());
        for (auto const idx : image.dimensions().range())
        {
            EXPECT_EQ(image[idx], loadedImage[idx]);
        }
    };

    EXPECT_TRUE(image.setDimensions(Rectangle{1U, 1U}));
    image[0U] = BGRAPixel{10U, 10U, 10U};
    EXPECT_TRUE(image.write(&(*writer)));
    checkImage("./32_000003.qoi");
    EXPECT_TRUE(image.setDimensions(Rectangle{2U, 3U}));
    image[2U] = BGRAPixel{10U, 10U, 10U};
    image[4U] = BGRAPixel{10U, 10U, 10U};
    EXPECT_TRUE(image.write(&(*writer)));
    checkImage("./32_000005.qoi");
    EXPECT_TRUE(image.setDimensions(Rectangle{3U, 1U}));
    image[0U] = BGRAPixel{10U, 10U, 10U};
    image[1U] = BGRAPixel{12U, 12U, 12U};
    EXPECT_TRUE(image.write(&(*writer)));
    checkImage("./32_000007.qoi");
}

} // namespace Terrahertz::UnitTests
