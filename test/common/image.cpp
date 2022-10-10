#include "THzImage/common/image.h"

#include "THzCommon/math/rectangle.h"
#include "THzImage/common/iImageReader.h"
#include "THzImage/common/iImageTransformer.h"
#include "THzImage/common/pixel.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct Common_Image : public testing::Test
{
    class MockTransformer : public IImageTransformer<BGRAPixel>
    {
    public:
        MOCK_METHOD(Rectangle, dimensions, (), (const, noexcept, override));
        MOCK_METHOD(bool, transform, (BGRAPixel & pixel), (noexcept, override));
        MOCK_METHOD(bool, skip, (), (noexcept, override));
        MOCK_METHOD(bool, reset, (), (noexcept, override));
    };

    class MockReader : public IImageReader<BGRAPixel>
    {
    public:
        MOCK_METHOD(bool, multipleImages, (), (const, noexcept, override));
        MOCK_METHOD(bool, init, (), (noexcept, override));
        MOCK_METHOD(Rectangle, dimensions, (), (const, noexcept, override));
        MOCK_METHOD(bool, read, (gsl::span<BGRAPixel> buffer), (noexcept, override));
        MOCK_METHOD(void, deinit, (), (noexcept, override));
    };

    Rectangle testDimensions{16, 24, 16U, 12U};
    BGRAImage sut{};
};

TEST_F(Common_Image, ImageConstructionCorrect)
{
    Rectangle const expectedDimensions{};
    EXPECT_EQ(sut.dimensions(), expectedDimensions);
}

TEST_F(Common_Image, SetDimensions)
{
    EXPECT_TRUE(sut.setDimensions(testDimensions));
    EXPECT_EQ(sut.dimensions(), testDimensions);
}

TEST_F(Common_Image, IndexAccess)
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

TEST_F(Common_Image, CreateViewOfEntireImage)
{
    EXPECT_TRUE(sut.setDimensions(testDimensions));
    auto const view = sut.view();
    EXPECT_EQ(view.basePointer(), &sut[0U]);
    testDimensions.upperLeftPoint.x = 0U;
    testDimensions.upperLeftPoint.y = 0U;
    EXPECT_EQ(view.imageDimensions(), testDimensions);
    EXPECT_EQ(view.region(), testDimensions);
}

TEST_F(Common_Image, CreateViewOfImageRegion)
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

TEST_F(Common_Image, StoreResultOfGivenNullptr) { EXPECT_FALSE(sut.storeResultOf(nullptr)); }

TEST_F(Common_Image, StoreResultOfTransformerResetFalse)
{
    MockTransformer transformer{};
    EXPECT_CALL(transformer, reset()).Times(1);
    EXPECT_FALSE(sut.storeResultOf(&transformer));
}

TEST_F(Common_Image, StoreResultOfTransformerWithDimensionsOfAreaNull)
{
    MockTransformer transformer{};
    EXPECT_CALL(transformer, reset()).Times(1).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(transformer, dimensions()).Times(1).WillRepeatedly(testing::Return(Rectangle{}));
    EXPECT_FALSE(sut.storeResultOf(&transformer));
}

TEST_F(Common_Image, StoreResultOfTransformingTooFewPixels)
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
    EXPECT_FALSE(sut.storeResultOf(&transformer));
}

TEST_F(Common_Image, StoreResultOfSuccess)
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
    EXPECT_FALSE(sut.storeResultOf(&transformer));
}

TEST_F(Common_Image, ReadGivenNullptr) { EXPECT_FALSE(sut.read(nullptr)); }

TEST_F(Common_Image, ReadInitFalse)
{
    MockReader reader{};
    EXPECT_CALL(reader, init()).WillOnce(testing::Return(false));
    EXPECT_FALSE(sut.read(&reader));
}

TEST_F(Common_Image, ReadReadFalse)
{
    MockReader reader{};
    EXPECT_CALL(reader, init()).WillOnce(testing::Return(true));
    EXPECT_CALL(reader, dimensions()).WillOnce(testing::Return(Rectangle{0, 0, 2U, 2U}));
    EXPECT_CALL(reader, read(testing::_)).WillOnce(testing::Return(false));
    EXPECT_CALL(reader, deinit()).Times(1);
    EXPECT_FALSE(sut.read(&reader));
}

TEST_F(Common_Image, ReadReadTrue)
{
    MockReader reader{};
    EXPECT_CALL(reader, init()).WillOnce(testing::Return(true));
    EXPECT_CALL(reader, dimensions()).WillOnce(testing::Return(Rectangle{0, 0, 2U, 2U}));
    EXPECT_CALL(reader, read(testing::_)).WillOnce(testing::Return(true));
    EXPECT_CALL(reader, deinit()).Times(1);
    EXPECT_TRUE(sut.read(&reader));
}

TEST_F(Common_Image, ReadGivenBufferHasCorrectSize)
{
    // As gmock has problems creating a MATCHER once templates get involved we do this by hand
    struct MyMockReader : public IImageReader<BGRAPixel>
    {
        Rectangle dim{0, 0, 4U, 4U};
        bool      multipleImages() const noexcept override { return true; }

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
    EXPECT_TRUE(sut.read(&reader));
}

} // namespace Terrahertz::UnitTests
