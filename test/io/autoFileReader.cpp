#include "THzImage/io/autoFileReader.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/io/bmpWriter.hpp"
#include "THzImage/io/pngWriter.hpp"
#include "THzImage/io/qoiWriter.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IOAutoFileReader : public testing::Test
{
    void SetUp() noexcept override
    {
        TestImageGenerator generator{Rectangle{16U, 16U}};
        (void)testImage.readFrom(generator);
    }

    BGRAImage testImage{};
};

TEST_F(IOAutoFileReader, DefaultConstruction)
{
    BGRAImage image{};

    AutoFile::Reader sut{};
    EXPECT_FALSE(sut.imagePresent());
    EXPECT_EQ(sut.dimensions(), Rectangle{});
    EXPECT_FALSE(image.readFrom(sut));
}

TEST_F(IOAutoFileReader, ReadBMPStrict)
{
    BMP::Writer writer{"autoTest.bmp", false};
    ASSERT_TRUE(testImage.writeTo(&writer));

    BGRAImage        image{};
    AutoFile::Reader sut{"autoTest.bmp", AutoFile::Reader::ExtensionMode::strict};
    EXPECT_TRUE(sut.imagePresent());
    EXPECT_TRUE(sut.readInto(image));
    EXPECT_EQ(testImage, image);
    EXPECT_FALSE(sut.imagePresent());
}

TEST_F(IOAutoFileReader, ReadPNGStrict)
{
    PNG::Writer writer{"autoTest.png"};
    ASSERT_TRUE(testImage.writeTo(&writer));

    BGRAImage        image{};
    AutoFile::Reader sut{"autoTest.png", AutoFile::Reader::ExtensionMode::strict};
    EXPECT_TRUE(sut.imagePresent());
    EXPECT_TRUE(sut.readInto(image));
    EXPECT_EQ(testImage, image);
    EXPECT_FALSE(sut.imagePresent());
}

TEST_F(IOAutoFileReader, ReadQOIStrict)
{
    QOI::Writer writer{"autoTest.qoi"};
    ASSERT_TRUE(testImage.writeTo(&writer));

    BGRAImage        image{};
    AutoFile::Reader sut{"autoTest.qoi", AutoFile::Reader::ExtensionMode::strict};
    EXPECT_TRUE(sut.imagePresent());
    EXPECT_TRUE(sut.readInto(image));
    EXPECT_EQ(testImage, image);
    EXPECT_FALSE(sut.imagePresent());
}

TEST_F(IOAutoFileReader, StrictModeFailure)
{
    QOI::Writer writer{"undercoverQOI.bmp"};
    ASSERT_TRUE(testImage.writeTo(&writer));

    BGRAImage        image{};
    AutoFile::Reader sut{"undercoverQOI.bmp", AutoFile::Reader::ExtensionMode::strict};
    EXPECT_TRUE(sut.imagePresent());
    EXPECT_FALSE(sut.readInto(image));
    EXPECT_FALSE(sut.imagePresent());
}

TEST_F(IOAutoFileReader, LenientOnKnownExtension)
{
    PNG::Writer writer{"undercoverPNG.bmp"};
    ASSERT_TRUE(testImage.writeTo(&writer));

    BGRAImage        image{};
    AutoFile::Reader sut{"undercoverPNG.bmp"};
    EXPECT_TRUE(sut.imagePresent());
    EXPECT_TRUE(sut.readInto(image));
    EXPECT_EQ(testImage, image);
    EXPECT_FALSE(sut.imagePresent());
}

TEST_F(IOAutoFileReader, LenientOnUnknownExtension)
{
    QOI::Writer writer{"undercoverQOI.txt"};
    ASSERT_TRUE(testImage.writeTo(&writer));

    BGRAImage        image{};
    AutoFile::Reader sut{"undercoverQOI.txt"};
    EXPECT_TRUE(sut.imagePresent());
    EXPECT_TRUE(sut.readInto(image));
    EXPECT_EQ(testImage, image);
    EXPECT_FALSE(sut.imagePresent());
}

TEST_F(IOAutoFileReader, Reset)
{
    QOI::Writer qoiWriter{"autoSeries.qoi"};
    PNG::Writer pngWriter{"autoSeries.png"};
    ASSERT_TRUE(testImage.writeTo(&qoiWriter));
    ASSERT_TRUE(testImage.writeTo(&pngWriter));

    BGRAImage        image0{};
    AutoFile::Reader sut{"autoSeries.qoi"};
    EXPECT_TRUE(sut.imagePresent());
    EXPECT_TRUE(sut.readInto(image0));
    EXPECT_EQ(testImage, image0);
    EXPECT_FALSE(sut.imagePresent());

    BGRAImage image1{};
    sut.reset("autoSeries.png");
    EXPECT_TRUE(sut.imagePresent());
    EXPECT_TRUE(sut.readInto(image1));
    EXPECT_EQ(testImage, image1);
    EXPECT_FALSE(sut.imagePresent());
}

TEST_F(IOAutoFileReader, FileWithNoExtension)
{
    QOI::Writer qoiWriter{"autoNoExtension"};
    ASSERT_TRUE(testImage.writeTo(&qoiWriter));

    BGRAImage        image{};
    AutoFile::Reader sut{"autoNoExtension", AutoFile::Reader::ExtensionMode::strict};
    EXPECT_TRUE(sut.imagePresent());
    EXPECT_FALSE(sut.readInto(image));
    EXPECT_FALSE(sut.imagePresent());

    sut.reset("autoNoExtension");
    EXPECT_TRUE(sut.imagePresent());
    EXPECT_TRUE(sut.readInto(image));
    EXPECT_FALSE(sut.imagePresent());
    EXPECT_EQ(testImage, image);
}

} // namespace Terrahertz::UnitTests
