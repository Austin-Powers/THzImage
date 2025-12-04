#include "THzImage/processing/easyWriter.hpp"

#include "THzImage/io/pngReader.hpp"
#include "THzImage/processing/dataReductionNode.hpp"
#include "THzImage/processing/testInputNode.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct ProcessingEasyWriter : public testing::Test
{
    Rectangle getDimensionsOf(std::string const path) noexcept
    {
        BGRAImage   image{};
        PNG::Reader reader{path};
        EXPECT_TRUE(image.readFrom(reader));
        return image.dimensions();
    }

    Rectangle const baseDimensions{24U, 31U};

    ImageProcessing::TestInputNode testNode{baseDimensions};

    using SutClass = ImageProcessing::EasyWriter;
};

TEST_F(ProcessingEasyWriter, CannotWriteIfPathIsInvalid)
{
    SutClass sut{"nodeWriterError/image_?.png"};
    EXPECT_TRUE(testNode.next());
    EXPECT_FALSE(sut.write(testNode[0U]));
}

TEST_F(ProcessingEasyWriter, BGRAImageIsWrittenCorrectly)
{
    std::filesystem::create_directories("nodeWriterBGRA");

    SutClass sut{"nodeWriterBGRA/image_?.png"};
    EXPECT_TRUE(testNode.next());
    EXPECT_TRUE(sut.write(testNode[0U]));
    EXPECT_EQ(getDimensionsOf("nodeWriterBGRA/image_000000.png"), baseDimensions);
    EXPECT_TRUE(sut.write(testNode[0U]));
    EXPECT_EQ(getDimensionsOf("nodeWriterBGRA/image_000001.png"), baseDimensions);

    std::filesystem::remove_all("nodeWriterBGRA");
}

TEST_F(ProcessingEasyWriter, NonBGRAImageIsWrittenCorrectly)
{
    std::filesystem::create_directories("nodeWriterNonBGRA");

    SutClass sut{"nodeWriterNonBGRA/image_?.png"};

    ImageProcessing::DataReductionNode reduction{testNode, std::uint8_t{1U}, 2U};
    EXPECT_TRUE(reduction.next());
    EXPECT_TRUE(sut.write(reduction[0U]));
    EXPECT_EQ(getDimensionsOf("nodeWriterNonBGRA/image_000000.png"), baseDimensions);
    EXPECT_TRUE(sut.write(reduction[0U]));
    EXPECT_EQ(getDimensionsOf("nodeWriterNonBGRA/image_000001.png"), baseDimensions);

    std::filesystem::remove_all("nodeWriterNonBGRA");
}

TEST_F(ProcessingEasyWriter, AppendWorksAsExpected) {}

} // namespace Terrahertz::UnitTests
