#include "THzImage/io/gifWriter.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_GIFWriter_ColorReduction : public testing::Test
{
    GIF::Internal::ColorReduction sut{};
};

TEST_F(IO_GIFWriter_ColorReduction, InitialStateCorrect)
{
    auto const colorTable = sut.colorTable();
    ASSERT_EQ(colorTable.size(), 2U);
    EXPECT_EQ(colorTable[0U], (BGRAPixel{0xFFU, 0xFFU, 0xFFU}));
    EXPECT_EQ(colorTable[1U], (BGRAPixel{}));
    for (auto i = 0U; i < 128U; ++i)
    {
        BGRAPixel pixel{static_cast<std::uint8_t>(i), static_cast<std::uint8_t>(i), static_cast<std::uint8_t>(i)};
        EXPECT_EQ(sut.convert(pixel), 1U);
    }
    for (auto i = 128U; i < 256U; ++i)
    {
        BGRAPixel pixel{static_cast<std::uint8_t>(i), static_cast<std::uint8_t>(i), static_cast<std::uint8_t>(i)};
        EXPECT_EQ(sut.convert(pixel), 0U);
    }
}

} // namespace Terrahertz::UnitTests
