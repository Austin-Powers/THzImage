#include "THzImage/common/image.hpp"

#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/iImageReader.hpp"
#include "THzImage/common/iImageTransformer.hpp"
#include "THzImage/common/iImageWriter.hpp"
#include "THzImage/common/pixel.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct CommonImage : public testing::Test
{
    class MockTransformer : public IImageTransformer<BGRAPixel>
    {
    public:
        MOCK_METHOD(Rectangle, dimensions, (), (const, noexcept, override));
        MOCK_METHOD(bool, transform, (BGRAPixel & pixel), (noexcept, override));
        MOCK_METHOD(bool, skip, (), (noexcept, override));
        MOCK_METHOD(bool, reset, (), (noexcept, override));
        MOCK_METHOD(bool, nextImage, (), (noexcept, override));
    };

    class MockReader : public IImageReader<BGRAPixel>
    {
    public:
        MOCK_METHOD(bool, imagePresent, (), (const, noexcept, override));
        MOCK_METHOD(bool, init, (), (noexcept, override));
        MOCK_METHOD(Rectangle, dimensions, (), (const, noexcept, override));
        MOCK_METHOD(bool, read, (gsl::span<BGRAPixel> buffer), (noexcept, override));
        MOCK_METHOD(void, deinit, (), (noexcept, override));
    };

    class MockWriter : public IImageWriter<BGRAPixel>
    {
    public:
        MOCK_METHOD(bool, init, (), (noexcept, override));
        MOCK_METHOD(bool,
                    write,
                    (Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer),
                    (noexcept, override));
        MOCK_METHOD(void, deinit, (), (noexcept, override));
    };

    Rectangle testDimensions{16, 24, 16U, 12U};
    BGRAImage sut{};
};

TEST_F(CommonImage, ImageConstructionCorrect)
{
    Rectangle const expectedDimensions{};
    EXPECT_EQ(sut.dimensions(), expectedDimensions);
}

TEST_F(CommonImage, SetDimensions)
{
    EXPECT_TRUE(sut.setDimensions(testDimensions));
    EXPECT_EQ(sut.dimensions(), testDimensions);
}

TEST_F(CommonImage, IndexAccess)
{
    EXPECT_TRUE(sut.setDimensions(testDimensions));
    BGRAPixel const expectedDefaultColor{};
    for (auto i = 0U; i < sut.dimensions().area(); ++i)
    {
        EXPECT_EQ(sut[i], expectedDefaultColor);
    }

    BGRAPixel const testColor{12U, 16U, 20U, 24U};
    sut[4U] = testColor;
    EXPECT_EQ(sut[4U], testColor);
}

TEST_F(CommonImage, CreateViewOfEntireImage)
{
    EXPECT_TRUE(sut.setDimensions(testDimensions));
    auto const view = sut.view();
    EXPECT_EQ(view.basePointer(), &sut[0U]);
    testDimensions.upperLeftPoint.x = 0U;
    testDimensions.upperLeftPoint.y = 0U;
    EXPECT_EQ(view.imageDimensions(), testDimensions);
    EXPECT_EQ(view.region(), testDimensions);
}

TEST_F(CommonImage, CreateViewOfImageRegion)
{
    EXPECT_TRUE(sut.setDimensions(testDimensions));
    Rectangle const region{2, 2, 4U, 4U};
    auto const      view = sut.view(region);
    EXPECT_EQ(view.basePointer(), &sut[0U]);
    testDimensions.upperLeftPoint.x = 0U;
    testDimensions.upperLeftPoint.y = 0U;
    EXPECT_EQ(view.imageDimensions(), testDimensions);
    EXPECT_EQ(view.region(), region);
}

TEST_F(CommonImage, ExecuteAndIngestTransformerResetFalse)
{
    MockTransformer transformer{};
    EXPECT_CALL(transformer, reset()).Times(1);
    EXPECT_FALSE(sut.executeAndIngest(transformer));
}

TEST_F(CommonImage, ExecuteAndIngestTransformerWithDimensionsOfAreaNull)
{
    MockTransformer transformer{};
    EXPECT_CALL(transformer, reset()).Times(1).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(transformer, dimensions()).Times(1).WillRepeatedly(testing::Return(Rectangle{}));
    EXPECT_FALSE(sut.executeAndIngest(transformer));
}

TEST_F(CommonImage, ExecuteAndIngestTransformingTooFewPixels)
{
    BGRAPixel       pix{};
    MockTransformer transformer{};
    EXPECT_CALL(transformer, reset()).Times(1).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(transformer, dimensions()).Times(1).WillRepeatedly(testing::Return(Rectangle{0, 0, 4U, 4U}));
    EXPECT_CALL(transformer, transform(pix))
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(false));
    EXPECT_FALSE(sut.executeAndIngest(transformer));
}

TEST_F(CommonImage, ExecuteAndIngestSuccess)
{
    BGRAPixel       pix{};
    MockTransformer transformer{};
    EXPECT_CALL(transformer, reset()).Times(1).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(transformer, dimensions()).Times(1).WillRepeatedly(testing::Return(Rectangle{0, 0, 2U, 2U}));
    EXPECT_CALL(transformer, transform(pix))
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(false));
    EXPECT_TRUE(sut.executeAndIngest(transformer));
}

TEST_F(CommonImage, ReadInitFalse)
{
    MockReader reader{};
    EXPECT_CALL(reader, init()).WillOnce(testing::Return(false));
    EXPECT_FALSE(sut.readFrom(reader));
}

TEST_F(CommonImage, ReadReadFalse)
{
    MockReader reader{};
    EXPECT_CALL(reader, init()).WillOnce(testing::Return(true));
    EXPECT_CALL(reader, dimensions()).WillOnce(testing::Return(Rectangle{0, 0, 2U, 2U}));
    EXPECT_CALL(reader, read(testing::_)).WillOnce(testing::Return(false));
    EXPECT_CALL(reader, deinit()).Times(1);
    EXPECT_FALSE(sut.readFrom(reader));
}

TEST_F(CommonImage, ReadReadTrue)
{
    MockReader reader{};
    EXPECT_CALL(reader, init()).WillOnce(testing::Return(true));
    EXPECT_CALL(reader, dimensions()).WillOnce(testing::Return(Rectangle{0, 0, 2U, 2U}));
    EXPECT_CALL(reader, read(testing::_)).WillOnce(testing::Return(true));
    EXPECT_CALL(reader, deinit()).Times(1);
    EXPECT_TRUE(sut.readFrom(reader));
}

TEST_F(CommonImage, ReadGivenBufferHasCorrectSize)
{
    // As gmock has problems creating a MATCHER once templates get involved we do this by hand
    struct MyMockReader : public IImageReader<BGRAPixel>
    {
        Rectangle dim{0, 0, 4U, 4U};
        bool      imagePresent() const noexcept override { return true; }

        bool init() noexcept override { return true; }

        Rectangle dimensions() const noexcept override { return dim; }

        bool read(gsl::span<BGRAPixel> buffer) noexcept override
        {
            EXPECT_EQ(dim.area(), buffer.size());
            return true;
        }

        void deinit() noexcept override {}
    };

    MyMockReader reader{};
    EXPECT_TRUE(sut.readFrom(reader));
}

TEST_F(CommonImage, WriteGivenNullptr) { EXPECT_FALSE(sut.writeTo(nullptr)); }

TEST_F(CommonImage, WriteWithNoDataInTheImage)
{
    MockWriter writer{};
    EXPECT_FALSE(sut.writeTo(&writer));
}

TEST_F(CommonImage, WriteInitFalse)
{
    EXPECT_TRUE(sut.setDimensions(Rectangle{0, 0, 4U, 4U}));
    MockWriter writer{};
    EXPECT_CALL(writer, init()).WillOnce(testing::Return(false));
    EXPECT_FALSE(sut.writeTo(&writer));
}

TEST_F(CommonImage, WriteWriteFalse)
{
    EXPECT_TRUE(sut.setDimensions(Rectangle{0, 0, 4U, 4U}));
    MockWriter writer{};
    EXPECT_CALL(writer, init()).WillOnce(testing::Return(true));
    EXPECT_CALL(writer, write(testing::_, testing::_)).WillOnce(testing::Return(false));
    EXPECT_CALL(writer, deinit()).Times(1);
    EXPECT_FALSE(sut.writeTo(&writer));
}

TEST_F(CommonImage, WriteWriteTrue)
{
    EXPECT_TRUE(sut.setDimensions(Rectangle{0, 0, 4U, 4U}));
    MockWriter writer{};
    EXPECT_CALL(writer, init()).WillOnce(testing::Return(true));
    EXPECT_CALL(writer, write(testing::_, testing::_)).WillOnce(testing::Return(true));
    EXPECT_CALL(writer, deinit()).Times(1);
    EXPECT_TRUE(sut.writeTo(&writer));
}

TEST_F(CommonImage, WriteGivenDataCorrect)
{
    // As gmock has problems creating a MATCHER once templates get involved we do this by hand
    struct MyMockWriter : public IImageWriter<BGRAPixel>
    {
        Rectangle expectedDimensions{0, 0, 4U, 4U};

        bool init() noexcept override { return true; }

        bool write(Rectangle const &dimensions, gsl::span<BGRAPixel const> buffer) noexcept override
        {
            BGRAPixel defaultPixel{};
            EXPECT_EQ(dimensions, expectedDimensions);
            EXPECT_EQ(buffer.size(), expectedDimensions.area());
            for (auto const &pixel : buffer)
            {
                EXPECT_EQ(pixel, defaultPixel);
            }
            return true;
        }

        void deinit() noexcept override {}
    };

    MyMockWriter writer{};
    EXPECT_TRUE(sut.setDimensions(writer.expectedDimensions));
    EXPECT_TRUE(sut.writeTo(&writer));
}

TEST_F(CommonImage, CopyViaExecuteAndIngest)
{
    BGRAImage orig{};
    EXPECT_TRUE(orig.setDimensions(Rectangle{2U, 2U}));
    BGRAImage dest{};
    auto      view = orig.view();
    EXPECT_TRUE(dest.executeAndIngest(view));
}

} // namespace Terrahertz::UnitTests
