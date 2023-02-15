#include "THzImage/handling/imageRingBuffer.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct Handling_ImageRingBuffer : public testing::Test
{
    struct TestReader : public IImageReader<BGRAPixel>
    {
        /// @copydoc IImageReader::imagePresent
        bool imagePresent() const noexcept override { return present; }

        /// @copydoc IImageReader::init
        bool init() noexcept override { return true; }

        /// @copydoc IImageReader::dimensions
        Rectangle dimensions() const noexcept override { return dim; }

        /// @copydoc IImageReader::read
        bool read(gsl::span<BGRAPixel> buffer) noexcept override
        {
            ++value;
            for (auto &pix : buffer)
            {
                pix.blue  = value;
                pix.green = value;
                pix.red   = value;
            }
            return true;
        }

        /// @copydoc IImageReader::deinit
        void deinit() noexcept override {}

        Rectangle dim{2U, 2U};

        bool present{true};

        std::uint8_t value{};
    };

    using TestSubject = ImageRingBuffer<BGRAPixel, 3U>;

    TestReader reader{};

    TestSubject sut{reader};
};

TEST_F(Handling_ImageRingBuffer, ConstructionCorrect)
{
    EXPECT_EQ(sut.slots(), 3U);

    EXPECT_EQ(sut[0U].dimensions(), Rectangle{});
    EXPECT_EQ(sut[1U].dimensions(), Rectangle{});
    EXPECT_EQ(sut[2U].dimensions(), Rectangle{});

    EXPECT_EQ(&sut[0U], &sut[0U]);
    EXPECT_NE(&sut[0U], &sut[1U]);
    EXPECT_NE(&sut[0U], &sut[2U]);
    EXPECT_NE(&sut[1U], &sut[2U]);
}

TEST_F(Handling_ImageRingBuffer, NextCalledUsingReader)
{
    auto const checkImage = [](BGRAImage const &image, std::uint8_t const value) noexcept {
        for (auto idx : image.dimensions().range())
        {
            EXPECT_EQ(image[idx].blue, value);
            EXPECT_EQ(image[idx].green, value);
            EXPECT_EQ(image[idx].red, value);
        }
    };

    EXPECT_TRUE(sut.next());
    checkImage(sut[0U], reader.value);

    EXPECT_TRUE(sut.next());
    checkImage(sut[0U], reader.value);
    checkImage(sut[1U], reader.value - 1U);

    EXPECT_TRUE(sut.next());
    checkImage(sut[0U], reader.value);
    checkImage(sut[1U], reader.value - 1U);
    checkImage(sut[2U], reader.value - 2U);

    reader.present = false;
    EXPECT_FALSE(sut.next());
}

TEST_F(Handling_ImageRingBuffer, NextCalledUsingTransformer)
{
    auto const checkImage = [](BGRAImage const &image, std::uint8_t const value) noexcept {
        for (auto idx : image.dimensions().range())
        {
            EXPECT_EQ(image[idx].blue, value);
            EXPECT_EQ(image[idx].green, value);
            EXPECT_EQ(image[idx].red, value);
        }
    };

    struct TestTransformer : public IImageTransformer<BGRAPixel>
    {
        Rectangle dimensions() const noexcept override { return dim; }

        bool transform(BGRAPixel &pixel) noexcept override
        {
            pixel.blue  = value;
            pixel.green = value;
            pixel.red   = value;
            return true;
        }

        bool skip() noexcept override { return true; }

        bool reset() noexcept override { return true; }

        bool nextImage() noexcept override
        {
            ++value;
            return next;
        }

        Rectangle dim{2U, 2U};

        bool next{true};

        std::uint8_t value{};
    };

    TestTransformer transformer{};

    TestSubject sut2{transformer};

    EXPECT_TRUE(sut2.next());
    checkImage(sut2[0U], transformer.value);

    EXPECT_TRUE(sut2.next());
    checkImage(sut2[0U], transformer.value);
    checkImage(sut2[1U], transformer.value - 1U);

    EXPECT_TRUE(sut2.next());
    checkImage(sut2[0U], transformer.value);
    checkImage(sut2[1U], transformer.value - 1U);
    checkImage(sut2[2U], transformer.value - 2U);

    transformer.next = false;
    EXPECT_FALSE(sut2.next());
}

} // namespace Terrahertz::UnitTests
