#include "THzImage/io/imageDirectoryReader.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/io/bmpWriter.hpp"
#include "THzImage/io/pngWriter.hpp"
#include "THzImage/io/qoiWriter.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <array>
#include <filesystem>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IOImageDirectoryReader : public testing::Test
{
    static void SetUpTestSuite()
    {
        BGRAImage          image{};
        TestImageGenerator generator{Rectangle{16U, 16U}};

        std::filesystem::create_directories("directoryReaderTest/subDir");
        // directoryReaderTest/subDir/containsImage
        PNG::Writer writer0{"directoryReaderTest/actuallyQoi.png"};
        image[0U].blue = 0x0FU;
        (void)writer0.writeContentOf(image);
        // directoryReaderTest/subDir/noImage.txt

        // directoryReaderTest/subDir/testQoi.qoi
        PNG::Writer writer1{"directoryReaderTest/subDir/testQoi.qoi"};
        image[0U].blue = 0x1FU;
        (void)writer1.writeContentOf(image);
        // directoryReaderTest/actuallyQoi.png
        PNG::Writer writer2{"directoryReaderTest/actuallyQoi.png"};
        image[0U].blue = 0x2FU;
        (void)writer2.writeContentOf(image);
        // directoryReaderTest/testBmp.bmp
        BMP::Writer writer3{"directoryReaderTest/testBmp.bmp"};
        image[0U].blue = 0x3FU;
        (void)writer3.writeContentOf(image);
        // directoryReaderTest/testPng.png
        PNG::Writer writer4{"directoryReaderTest/testPng.png"};
        image[0U].blue = 0x4FU;
        (void)writer4.writeContentOf(image);
    };

    static void TearDownTestSuite() { std::filesystem::remove_all("directoryReaderTest"); }
};

TEST_F(IOImageDirectoryReader, EmptyFolder)
{
    std::string const directoryName = "noImagesHere";
    std::filesystem::create_directory(directoryName);
    BGRAImage image{};

    ImageDirectory::Reader sut{directoryName, ImageDirectory::Reader::Mode::automatic};

    EXPECT_FALSE(sut.imagePresent());
    EXPECT_FALSE(sut.readInto(image));

    std::filesystem::remove_all(directoryName);
}

TEST_F(IOImageDirectoryReader, StrictExtensionBasedMode) {}

TEST_F(IOImageDirectoryReader, ExtensionBasedMode) {}

TEST_F(IOImageDirectoryReader, AutomaticMode) {}

} // namespace Terrahertz::UnitTests
