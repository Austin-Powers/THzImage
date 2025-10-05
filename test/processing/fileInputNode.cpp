#include "THzImage/processing/fileInputNode.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/io/bmpWriter.hpp"
#include "THzImage/io/pngWriter.hpp"
#include "THzImage/io/qoiWriter.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>
#include <string>

namespace Terrahertz::UnitTests {

struct ProcessingFileInputNode : public testing::Test
{
    static void SetUpTestSuite()
    {
        BGRAImage          image{};
        TestImageGenerator generator{Rectangle{16U, 16U}};
        (void)image.readFrom(generator);

        std::filesystem::create_directories("fileNodeTest/subDir");
        // fileNodeTest/subDir/containsImage
        PNG::Writer writer0{"fileNodeTest/subDir/containsImage"};
        image[0U].blue = 0x0FU;
        (void)writer0.writeContentOf(image);
        // fileNodeTest/subDir/noImage.txt
        {
            std::ofstream stream{"fileNodeTest/subDir/noImage.txt"};
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
        // fileNodeTest/subDir/testQoi.qoi
        QOI::Writer writer1{"fileNodeTest/subDir/testQoi.qoi"};
        image[0U].blue = 0x1FU;
        (void)writer1.writeContentOf(image);
        // fileNodeTest/actuallyQoi.png
        QOI::Writer writer2{"fileNodeTest/actuallyQoi.png"};
        image[0U].blue = 0x2FU;
        (void)writer2.writeContentOf(image);
        // fileNodeTest/testBmp.bmp
        BMP::Writer writer3{"fileNodeTest/testBmp.bmp"};
        image[0U].blue = 0x3FU;
        (void)writer3.writeContentOf(image);
        // fileNodeTest/testPng.png
        PNG::Writer writer4{"fileNodeTest/testPng.png"};
        image[0U].blue = 0x4FU;
        (void)writer4.writeContentOf(image);
    };

    static void TearDownTestSuite() { std::filesystem::remove_all("fileNodeTest"); }
};

TEST_F(ProcessingFileInputNode, NothingLoadedBeforeFirstCallToNext)
{
    auto const bufferSize = 3U;

    ImageProcessing::FileInputNode sut{bufferSize, "fileNodeTest"};

    Rectangle const             expectedDimensions{};
    std::filesystem::path const expectedPath{};

    EXPECT_EQ(sut.slots(), bufferSize);
    EXPECT_EQ(sut.count(), 0U);
    for (auto i = 0U; i < bufferSize; ++i)
    {
        EXPECT_EQ(sut[i].dimensions(), expectedDimensions);
        EXPECT_EQ(sut.pathOf(i), expectedPath);
    }
}

TEST_F(ProcessingFileInputNode, IndexOutOfBoundsReturnsEmpty)
{
    ImageProcessing::FileInputNode sut{1U, "fileNodeTest"};
    Rectangle const                expectedDimensions{};
    std::filesystem::path const    expectedPath{};
    EXPECT_EQ(sut[1U].dimensions(), expectedDimensions);
    EXPECT_EQ(sut.pathOf(1U), expectedPath);
}

TEST_F(ProcessingFileInputNode, Automatic)
{
    ImageProcessing::FileInputNode sut{10U, "fileNodeTest"};
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut.count(), 1U);
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut.count(), 2U);
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut.count(), 3U);
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut.count(), 4U);
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut.count(), 5U);
    EXPECT_FALSE(sut.next());
    EXPECT_EQ(sut.count(), 5U);
    EXPECT_FALSE(sut.next());
    EXPECT_EQ(sut.count(), 5U);

    for (auto i = 0U; i < sut.slots(); ++i)
    {
        if (sut[i].dimensions().area() != 0U)
        {
            switch (sut[i][0U].blue)
            {
            case 0x0FU:
                EXPECT_EQ(sut.pathOf(i), std::filesystem::path{"fileNodeTest/subDir/containsImage"});
                break;
            case 0x1FU:
                EXPECT_EQ(sut.pathOf(i), std::filesystem::path{"fileNodeTest/subDir/testQoi.qoi"});
                break;
            case 0x2FU:
                EXPECT_EQ(sut.pathOf(i), std::filesystem::path{"fileNodeTest/actuallyQoi.png"});
                break;
            case 0x3FU:
                EXPECT_EQ(sut.pathOf(i), std::filesystem::path{"fileNodeTest/testBmp.bmp"});
                break;
            case 0x4FU:
                EXPECT_EQ(sut.pathOf(i), std::filesystem::path{"fileNodeTest/testPng.png"});
                break;
            default:
                ADD_FAILURE() << "Unknown file loaded";
                break;
            }
        }
    }
}

TEST_F(ProcessingFileInputNode, ExtensionBased)
{
    ImageProcessing::FileInputNode sut{10U, "fileNodeTest", ImageProcessing::FileInputNode::Mode::extensionBased};
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut.count(), 1U);
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut.count(), 2U);
    EXPECT_TRUE(sut.next());
    EXPECT_EQ(sut.count(), 3U);
    EXPECT_FALSE(sut.next());
    EXPECT_EQ(sut.count(), 3U);
    EXPECT_FALSE(sut.next());
    EXPECT_EQ(sut.count(), 3U);

    for (auto i = 0U; i < sut.slots(); ++i)
    {
        if (sut[i].dimensions().area() != 0U)
        {
            switch (sut[i][0U].blue)
            {
            case 0x1FU:
                EXPECT_EQ(sut.pathOf(i), std::filesystem::path{"fileNodeTest/subDir/testQoi.qoi"});
                break;
            case 0x3FU:
                EXPECT_EQ(sut.pathOf(i), std::filesystem::path{"fileNodeTest/testBmp.bmp"});
                break;
            case 0x4FU:
                EXPECT_EQ(sut.pathOf(i), std::filesystem::path{"fileNodeTest/testPng.png"});
                break;
            default:
                ADD_FAILURE() << "Unknown file loaded";
                break;
            }
        }
    }
}

TEST_F(ProcessingFileInputNode, StrictExtensionBased)
{
    ImageProcessing::FileInputNode sut{10U, "fileNodeTest", ImageProcessing::FileInputNode::Mode::strictExtensionBased};

    uint8_t successCounter = 0U;
    for (auto i = 0U; i < 10U; ++i)
    {
        if (sut.next())
        {
            ++successCounter;
        }
    }
    EXPECT_EQ(successCounter, 3U);
    for (auto i = 0U; i < sut.slots(); ++i)
    {
        if (sut[i].dimensions().area() != 0U)
        {
            switch (sut[i][0U].blue)
            {
            case 0x1FU:
                EXPECT_EQ(sut.pathOf(i), std::filesystem::path{"fileNodeTest/subDir/testQoi.qoi"});
                break;
            case 0x3FU:
                EXPECT_EQ(sut.pathOf(i), std::filesystem::path{"fileNodeTest/testBmp.bmp"});
                break;
            case 0x4FU:
                EXPECT_EQ(sut.pathOf(i), std::filesystem::path{"fileNodeTest/testPng.png"});
                break;
            default:
                ADD_FAILURE() << "Unknown file loaded";
                break;
            }
        }
    }
}

} // namespace Terrahertz::UnitTests
