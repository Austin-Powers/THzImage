#include "THzImage/transformation/pixelTransformer.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/common/pixel.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct TransformationPixelTransformer : public testing::Test
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

    struct TestTransformation
    {
        BGRAPixel operator()(BGRAPixel pixel) noexcept
        {
            pixel.red = 10U;
            return pixel;
        }
    };

    void SetUp() override
    {
        TestImageGenerator generator{Rectangle{20U, 20U}};
        EXPECT_TRUE(imageBase.readFrom(generator));
        baseTransformer = imageBase.view();
    }

    BGRAImage imageBase{};

    BGRAImage imageReceiver{};

    ImageView<BGRAPixel> baseTransformer{};
};

TEST_F(TransformationPixelTransformer, DefaultCreateTransformerFromInstance)
{
    auto transformer = createPixelTransformer<BGRAPixel, TestTransformation>(baseTransformer);
    EXPECT_TRUE(imageReceiver.executeAndIngest(transformer));
    for (auto i : imageBase.dimensions().range())
    {
        EXPECT_EQ(imageReceiver[i].blue, imageBase[i].blue);
        EXPECT_EQ(imageReceiver[i].green, imageBase[i].green);
        EXPECT_EQ(imageReceiver[i].red, 10U);
    }
}

TEST_F(TransformationPixelTransformer, CreateTransformerFromInstance)
{
    auto transformer = createPixelTransformer<BGRAPixel>(baseTransformer, TestTransformation{});
    EXPECT_TRUE(imageReceiver.executeAndIngest(transformer));
    for (auto i : imageBase.dimensions().range())
    {
        EXPECT_EQ(imageReceiver[i].blue, imageBase[i].blue);
        EXPECT_EQ(imageReceiver[i].green, imageBase[i].green);
        EXPECT_EQ(imageReceiver[i].red, 10U);
    }
}

TEST_F(TransformationPixelTransformer, CreateTransformerFromLambda)
{
    auto transformer =
        createPixelTransformer<BGRAPixel>(baseTransformer, [](BGRAPixel const &pixel) noexcept -> BGRAPixel {
            auto p = pixel;
            p.blue = 0U;
            return p;
        });
    EXPECT_TRUE(imageReceiver.executeAndIngest(transformer));
    for (auto i : imageBase.dimensions().range())
    {
        EXPECT_EQ(imageReceiver[i].blue, 0U);
        EXPECT_EQ(imageReceiver[i].green, imageBase[i].green);
        EXPECT_EQ(imageReceiver[i].red, imageBase[i].red);
    }
}

TEST_F(TransformationPixelTransformer, NextImageCallHandedToBase)
{
    MockTransformer baseTransformer{};

    auto transformer = createPixelTransformer<BGRAPixel>(baseTransformer, TestTransformation{});
    EXPECT_CALL(baseTransformer, nextImage()).Times(2).WillOnce(testing::Return(true)).WillOnce(testing::Return(false));
    EXPECT_TRUE(transformer.nextImage());
    EXPECT_FALSE(transformer.nextImage());
}

} // namespace Terrahertz::UnitTests
