#include "THzImage/transformation/pixelTransformer.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/common/pixel.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct Transformation_PixelTransformer : public testing::Test
{
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
        EXPECT_TRUE(imageBase.read(&generator));
        baseTransformer = imageBase.view();
    }

    BGRAImage imageBase{};

    BGRAImage imageReceiver{};

    ImageView<BGRAPixel> baseTransformer{};
};

TEST_F(Transformation_PixelTransformer, DefaultCreateTransformerFromInstance)
{
    auto transformer = createPixelTransformer<BGRAPixel, TestTransformation>(baseTransformer);
    EXPECT_TRUE(imageReceiver.storeResultOf(&transformer));
    for (auto i : imageBase.dimensions().range())
    {
        EXPECT_EQ(imageReceiver[i].blue, imageBase[i].blue);
        EXPECT_EQ(imageReceiver[i].green, imageBase[i].green);
        EXPECT_EQ(imageReceiver[i].red, 10U);
    }
}

TEST_F(Transformation_PixelTransformer, CreateTransformerFromInstance)
{
    auto transformer = createPixelTransformer<BGRAPixel>(baseTransformer, TestTransformation{});
    EXPECT_TRUE(imageReceiver.storeResultOf(&transformer));
    for (auto i : imageBase.dimensions().range())
    {
        EXPECT_EQ(imageReceiver[i].blue, imageBase[i].blue);
        EXPECT_EQ(imageReceiver[i].green, imageBase[i].green);
        EXPECT_EQ(imageReceiver[i].red, 10U);
    }
}

TEST_F(Transformation_PixelTransformer, CreateTransformerFromLambda)
{
    auto transformer =
        createPixelTransformer<BGRAPixel>(baseTransformer, [](BGRAPixel const &pixel) noexcept -> BGRAPixel {
            auto p = pixel;
            p.blue = 0U;
            return p;
        });
    EXPECT_TRUE(imageReceiver.storeResultOf(&transformer));
    for (auto i : imageBase.dimensions().range())
    {
        EXPECT_EQ(imageReceiver[i].blue, 0U);
        EXPECT_EQ(imageReceiver[i].green, imageBase[i].green);
        EXPECT_EQ(imageReceiver[i].red, imageBase[i].red);
    }
}

} // namespace Terrahertz::UnitTests
