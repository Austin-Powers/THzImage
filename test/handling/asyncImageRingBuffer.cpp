#include "THzImage/handling/asyncImageRingBuffer.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct Handling_AsyncImageRingBuffer : public testing::Test
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

    using TestSubject = AsyncImageRingBuffer<BGRAPixel>;

    TestReader reader{};

    TestSubject sut{reader, 3U};
};

TEST_F(Handling_AsyncImageRingBuffer, ConstructionCorrect)
{
    EXPECT_EQ(sut.slots(), 3U);
    EXPECT_EQ(sut.count(), 0U);

    EXPECT_EQ(sut[0U].dimensions(), Rectangle{});
    EXPECT_EQ(sut[1U].dimensions(), Rectangle{});
    EXPECT_EQ(sut[2U].dimensions(), Rectangle{});

    EXPECT_EQ(&sut[0U], &sut[0U]);
    EXPECT_NE(&sut[0U], &sut[1U]);
    EXPECT_NE(&sut[0U], &sut[2U]);
    EXPECT_NE(&sut[1U], &sut[2U]);
}

TEST_F(Handling_AsyncImageRingBuffer, NextCalledUsingReader)
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
    checkImage(sut[0U], 1U);
    EXPECT_EQ(sut.count(), 1U);

    EXPECT_TRUE(sut.next());
    checkImage(sut[0U], 2U);
    checkImage(sut[1U], 1U);
    EXPECT_EQ(sut.count(), 2U);

    EXPECT_TRUE(sut.next());
    checkImage(sut[0U], 3U);
    checkImage(sut[1U], 2U);
    checkImage(sut[2U], 1U);
    EXPECT_EQ(sut.count(), 3U);

    EXPECT_TRUE(sut.next());
    checkImage(sut[0U], 4U);
    checkImage(sut[1U], 3U);
    checkImage(sut[2U], 2U);
    EXPECT_EQ(sut.count(), 4U);

    EXPECT_TRUE(sut.next());
    checkImage(sut[0U], 5U);
    checkImage(sut[1U], 4U);
    checkImage(sut[2U], 3U);
    EXPECT_EQ(sut.count(), 5U);

    reader.present = false;
    // ignore next result as outcome is highly dependent on timing
    auto const result = sut.next();
    EXPECT_FALSE(sut.next());
}

TEST_F(Handling_AsyncImageRingBuffer, NextCalledUsingTransformer)
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

    TestSubject sut2{transformer, 3U};

    EXPECT_TRUE(sut2.next());
    checkImage(sut2[0U], 0U);
    EXPECT_EQ(sut2.count(), 1U);

    EXPECT_TRUE(sut2.next());
    checkImage(sut2[0U], 1U);
    checkImage(sut2[1U], 0U);
    EXPECT_EQ(sut2.count(), 2U);

    EXPECT_TRUE(sut2.next());
    checkImage(sut2[0U], 2U);
    checkImage(sut2[1U], 1U);
    checkImage(sut2[2U], 0U);
    EXPECT_EQ(sut2.count(), 3U);

    transformer.next = false;
    // ignore next result as outcome is highly dependent on timing
    auto const result = sut2.next();
    EXPECT_FALSE(sut2.next());
}

} // namespace Terrahertz::UnitTests
