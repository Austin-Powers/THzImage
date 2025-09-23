#include "THzImage/io/imageDirectoryReader.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/io/bmpWriter.hpp"
#include "THzImage/io/pngWriter.hpp"
#include "THzImage/io/qoiWriter.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IOImageDirectoryReader : public testing::Test
{
    static void SetUpTestSuite()
    {
        BGRAImage          image{};
        TestImageGenerator generator{Rectangle{16U, 16U}};
        (void)image.readFrom(generator);

        std::filesystem::create_directories("directoryReaderTest/subDir");
        // directoryReaderTest/subDir/containsImage
        PNG::Writer writer0{"directoryReaderTest/subDir/containsImage"};
        image[0U].blue = 0x0FU;
        (void)writer0.writeContentOf(image);
        // directoryReaderTest/subDir/noImage.txt
        {
            std::ofstream stream{"directoryReaderTest/subDir/noImage.txt"};
            if (!stream.is_open())
            {
                stream << "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed eiusmod tempor incidunt ut "
                          "labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco "
                          "laboris nisi ut aliquid ex ea commodi consequat. Quis aute iure reprehenderit in voluptate "
                          "velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint obcaecat cupiditat non "
                          "proident, sunt in culpa qui officia deserunt mollit anim id est laborum."
                       << std::endl;
            }
        }
        // directoryReaderTest/subDir/testQoi.qoi
        QOI::Writer writer1{"directoryReaderTest/subDir/testQoi.qoi"};
        image[0U].blue = 0x1FU;
        (void)writer1.writeContentOf(image);
        // directoryReaderTest/actuallyQoi.png
        QOI::Writer writer2{"directoryReaderTest/actuallyQoi.png"};
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

TEST_F(IOImageDirectoryReader, StrictExtensionBasedMode)
{
    BGRAImage image{};

    ImageDirectory::Reader sut{"directoryReaderTest", ImageDirectory::Reader::Mode::strictExtensionBased};

    std::uint8_t imageCounter = 0U;
    while (sut.imagePresent())
    {
        if (sut.readInto(image))
        {
            switch (image[0U].blue)
            {
            case 0x0FU:
                ADD_FAILURE() << "Image loaded that has no extension";
                ++imageCounter;
                break;
            case 0x1FU:
                EXPECT_EQ(sut.pathOfLastImage(), std::filesystem::path{"directoryReaderTest/subDir/testQoi.qoi"});
                ++imageCounter;
                break;
            case 0x2FU:
                ADD_FAILURE() << "Image loaded with wrong extension";
                ++imageCounter;
                break;
            case 0x3FU:
                EXPECT_EQ(sut.pathOfLastImage(), std::filesystem::path{"directoryReaderTest/testBmp.bmp"});
                ++imageCounter;
                break;
            case 0x4FU:
                EXPECT_EQ(sut.pathOfLastImage(), std::filesystem::path{"directoryReaderTest/testPng.png"});
                ++imageCounter;
                break;
            default:
                ADD_FAILURE() << "Unknown file loaded";
                break;
            }
        }
        else
        {
            EXPECT_EQ(sut.pathOfLastImage(), std::filesystem::path{"directoryReaderTest/actuallyQoi.png"});
        }
    }
    EXPECT_EQ(imageCounter, 3U);
}

TEST_F(IOImageDirectoryReader, ExtensionBasedMode)
{
    BGRAImage image{};

    ImageDirectory::Reader sut{"directoryReaderTest", ImageDirectory::Reader::Mode::extensionBased};

    std::uint8_t imageCounter = 0U;
    while (sut.imagePresent())
    {
        if (sut.readInto(image))
        {
            switch (image[0U].blue)
            {
            case 0x0FU:
                ADD_FAILURE() << "Image loaded that has no extension";
                ++imageCounter;
                break;
            case 0x1FU:
                EXPECT_EQ(sut.pathOfLastImage(), std::filesystem::path{"directoryReaderTest/subDir/testQoi.qoi"});
                ++imageCounter;
                break;
            case 0x2FU:
                ADD_FAILURE() << "Image loaded with wrong extension";
                ++imageCounter;
                break;
            case 0x3FU:
                EXPECT_EQ(sut.pathOfLastImage(), std::filesystem::path{"directoryReaderTest/testBmp.bmp"});
                ++imageCounter;
                break;
            case 0x4FU:
                EXPECT_EQ(sut.pathOfLastImage(), std::filesystem::path{"directoryReaderTest/testPng.png"});
                ++imageCounter;
                break;
            default:
                ADD_FAILURE() << "Unknown file loaded";
                break;
            }
        }
    }
    EXPECT_EQ(imageCounter, 3U);
}

TEST_F(IOImageDirectoryReader, AutomaticMode)
{
    BGRAImage image{};

    ImageDirectory::Reader sut{"directoryReaderTest"};

    std::uint8_t imageCounter = 0U;
    while (sut.imagePresent())
    {
        if (sut.readInto(image))
        {
            switch (image[0U].blue)
            {
            case 0x0FU:
                EXPECT_EQ(sut.pathOfLastImage(), std::filesystem::path{"directoryReaderTest/subDir/containsImage"});
                ++imageCounter;
                break;
            case 0x1FU:
                EXPECT_EQ(sut.pathOfLastImage(), std::filesystem::path{"directoryReaderTest/subDir/testQoi.qoi"});
                ++imageCounter;
                break;
            case 0x2FU:
                EXPECT_EQ(sut.pathOfLastImage(), std::filesystem::path{"directoryReaderTest/actuallyQoi.png"});
                ++imageCounter;
                break;
            case 0x3FU:
                EXPECT_EQ(sut.pathOfLastImage(), std::filesystem::path{"directoryReaderTest/testBmp.bmp"});
                ++imageCounter;
                break;
            case 0x4FU:
                EXPECT_EQ(sut.pathOfLastImage(), std::filesystem::path{"directoryReaderTest/testPng.png"});
                ++imageCounter;
                break;
            default:
                ADD_FAILURE() << "Unknown file loaded";
                break;
            }
        }
    }
    EXPECT_EQ(imageCounter, 5U);
}

} // namespace Terrahertz::UnitTests
