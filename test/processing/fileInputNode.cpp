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

    static constexpr char const *testDirectory() noexcept { return "fileNodeTestImages"; }

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
        std::filesystem::create_directory(testDirectory());

        BGRAImage          testImage{};
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
    }

    static void TearDownTestSuite()
    {
        // TODO Delete folder and test files
    }
};

TEST_F(ProcessingFileInputNode, NothingLoadedBeforeFirstCallToNext)
{
    ImageProcessing::FileInputNode sut{2U, testDirectory()};
}

} // namespace Terrahertz::UnitTests
