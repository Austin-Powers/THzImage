#include "THzImage/io/imageDirectoryReader.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/io/bmpWriter.hpp"
#include "THzImage/io/pngWriter.hpp"
#include "THzImage/io/qoiWriter.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <filesystem>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IOImageDirectoryReader : public testing::Test
{
    static constexpr char const *testDirectory() noexcept { return "directoryReaderTest"; }

    static constexpr char const *bmpFilename() noexcept { return "bmpImage.bmp"; }

    static constexpr uint8_t bmpBlueValue = 0xFFU;

    static constexpr char const *pngFilename() noexcept { return "pngImage.png"; }

    static constexpr uint8_t pngBlueValue = 0x55U;

    static constexpr char const *qoiFilename() noexcept { return "qoiImage.qoi"; }

    static constexpr uint8_t qoiBlueValue = 0x11U;

    static std::filesystem::path createPathFor(char const *const filename) noexcept
    {
        std::filesystem::path path{testDirectory()};
        path /= std::filesystem::path{filename};
        return path;
    }

    static void SetUpTestSuite()
    {
        BGRAImage testImage{};
        std::filesystem::create_directory(testDirectory());
        TestImageGenerator generator{Rectangle{16U, 16U}};
        (void)generator.readInto(testImage);

        testImage[0U].blue = bmpBlueValue;
        BMP::Writer bmpWriter{createPathFor(bmpFilename())};
        (void)testImage.writeTo(&bmpWriter);
        testImage[0U].blue = pngBlueValue;
        PNG::Writer pngWriter{createPathFor(pngFilename())};
        (void)testImage.writeTo(&pngWriter);
        testImage[0U].blue = qoiBlueValue;
        QOI::Writer qoiWriter{createPathFor(qoiFilename())};
        (void)testImage.writeTo(&qoiWriter);
    };

    static void TearDownTestSuite() { std::filesystem::remove_all(testDirectory()); }
};
/*
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

    ImageDirectory::Reader sut{testDirectory(), ImageDirectory::Reader::Mode::strictExtensionBased};

    // read BMP
    EXPECT_TRUE(sut.imagePresent());
    EXPECT_TRUE(sut.readInto(image));

    // read PNG
    // read QOI
    // fail
}

TEST_F(IOImageDirectoryReader, ExtensionBasedMode) {}

TEST_F(IOImageDirectoryReader, AutomaticMode) {}

TEST_F(IOImageDirectoryReader, HandlingSubDirectories) {}
*/
} // namespace Terrahertz::UnitTests
