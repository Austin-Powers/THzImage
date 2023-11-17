#include "THzImage/transformation/convolutionTransformer.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/common/pixel.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

namespace Terrahertz::UnitTests {

struct Transformation_ConvolutionTransformer : public testing::Test
{
    using BGRACParameters = ConvolutionParameters<BGRAPixel>;

    class MockTransformer : public IImageTransformer<BGRAPixel>
    {
    public:
        MOCK_METHOD(Rectangle, dimensions, (), (const, noexcept, override));
        MOCK_METHOD(bool, transform, (BGRAPixel & pixel), (noexcept, override));
        MOCK_METHOD(bool, skip, (), (noexcept, override));
        MOCK_METHOD(bool, reset, (), (noexcept, override));
        MOCK_METHOD(bool, nextImage, (), (noexcept, override));
    };

    MockTransformer baseTransformer{};

    class MockTransformation
    {
    public:
        struct Data
        {
            BGRACParameters parameters{3U, 3U, 1U, 1U, true, BGRAPixel{123U, 124U, 125U}};

            bool parametersCalled{};
        };

        BGRACParameters parameters() noexcept
        {
            data->parametersCalled = true;
            return data->parameters;
        }

        BGRAPixel operator()(BGRAPixel **const matrix) noexcept { return matrix[0U][0U]; }

        Data *data{};
    };

    using ClassUnderTest = ConvolutionTransformer<BGRAPixel, MockTransformation>;

    MockTransformation::Data transformationData{};

    MockTransformation transformation{};

    BGRAPixel const borderFill{0x12U, 0x53U, 0x25U, 0x02U};

    struct TestSetup
    {
        bool          border;
        std::uint16_t sizeX;
        std::uint16_t sizeY;
        std::uint16_t shiftX;
        std::uint16_t shiftY;
        std::uint16_t imageSizeX;
        std::uint16_t imageSizeY;
    };

    std::vector<TestSetup> setups{};

    void SetUp() override
    {
        transformation.data = &transformationData;
        for (auto border = 0U; border < 2U; ++border)
        {
            for (auto sizeX = 1U; sizeX < 4U; ++sizeX)
            {
                for (auto sizeY = 1U; sizeY < 4U; ++sizeY)
                {
                    for (auto shiftX = 1U; shiftX < 4U; ++shiftX)
                    {
                        for (auto shiftY = 1U; shiftY < 4U; ++shiftY)
                        {
                            for (auto imageSizeX = 1U; imageSizeX < 10U; imageSizeX += 3U)
                            {
                                for (auto imageSizeY = 1U; imageSizeY < 10U; imageSizeY += 3U)
                                {
                                    setups.emplace_back(
                                        border != 0, sizeX, sizeY, shiftX, shiftY, imageSizeX, imageSizeY);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
};

TEST_F(Transformation_ConvolutionTransformer, ParameterConstructionThrowIfGivenInvalidValues)
{
    BGRAPixel const pixel{};
    EXPECT_THROW(BGRACParameters wrongSizeX(0U, 1U, 1U, 1U, false, pixel), std::invalid_argument);
    EXPECT_THROW(BGRACParameters wrongSizeX(1U, 0U, 1U, 1U, false, pixel), std::invalid_argument);
    EXPECT_THROW(BGRACParameters wrongSizeX(1U, 1U, 0U, 1U, false, pixel), std::invalid_argument);
    EXPECT_THROW(BGRACParameters wrongSizeX(1U, 1U, 1U, 0U, false, pixel), std::invalid_argument);
}

TEST_F(Transformation_ConvolutionTransformer, ParametersReturnCorrectValues)
{
    std::uint16_t const sizeX{7U};
    std::uint16_t const sizeY{5U};
    std::uint16_t const shiftX{2U};
    std::uint16_t const shiftY{1U};
    bool const          border{true};

    ConvolutionParameters const parameters{sizeX, sizeY, shiftX, shiftY, border, borderFill};
    EXPECT_EQ(parameters.sizeX(), sizeX);
    EXPECT_EQ(parameters.sizeY(), sizeY);
    EXPECT_EQ(parameters.shiftX(), shiftX);
    EXPECT_EQ(parameters.shiftY(), shiftY);
    EXPECT_EQ(parameters.border(), border);
    EXPECT_EQ(parameters.borderFill(), borderFill);
}

TEST_F(Transformation_ConvolutionTransformer, NextImageIsPassedThroughCorrectly)
{
    Rectangle const testDimensions{160U, 128U};
    // called once construction and when next image is successful
    EXPECT_CALL(baseTransformer, dimensions()).Times(2).WillRepeatedly(testing::Return(testDimensions));
    EXPECT_CALL(baseTransformer, nextImage()).Times(2).WillOnce(testing::Return(true)).WillOnce(testing::Return(false));
    ClassUnderTest sut{baseTransformer, transformation};
    EXPECT_TRUE(sut.nextImage());
    EXPECT_FALSE(sut.nextImage());
}

TEST_F(Transformation_ConvolutionTransformer, ResetIsPassedThroughCorrectly)
{
    Rectangle const testDimensions{160U, 128U};
    // called once construction and when next image is successful
    EXPECT_CALL(baseTransformer, dimensions()).Times(2).WillRepeatedly(testing::Return(testDimensions));
    EXPECT_CALL(baseTransformer, reset()).Times(2).WillOnce(testing::Return(true)).WillOnce(testing::Return(false));
    ClassUnderTest sut{baseTransformer, transformation};
    EXPECT_TRUE(sut.reset());
    EXPECT_FALSE(sut.reset());
}

TEST_F(Transformation_ConvolutionTransformer, DISABLED_DimensionsReturnsExpectedResults)
{
    auto const expectedDimension = [](std::uint16_t const image,
                                      std::uint16_t const matrix,
                                      std::uint16_t const stepSize,
                                      bool const          border) noexcept -> std::uint32_t {
        std::uint32_t result{};

        auto remainingImage = image;
        if (border && (stepSize == 1U))
        {
            remainingImage += (matrix - 1U);
        }

        if (remainingImage >= matrix)
        {
            ++result;
            remainingImage -= matrix;
        }
        while (remainingImage >= stepSize)
        {
            ++result;
            remainingImage -= stepSize;
        }

        if (border && (stepSize != 1U) && (remainingImage != 0))
        {
            ++result;
        }
        return result;
    };

    for (auto const &setup : setups)
    {
        transformationData.parametersCalled = false;
        transformationData.parameters =
            BGRACParameters{setup.sizeX, setup.sizeY, setup.shiftX, setup.shiftY, setup.border, borderFill};
        Rectangle imageDimensions{setup.imageSizeX, setup.imageSizeY};
        EXPECT_CALL(baseTransformer, dimensions()).Times(1).WillOnce(testing::Return(imageDimensions));
        ClassUnderTest sut{baseTransformer, transformation};

        auto const result = sut.dimensions();
        ASSERT_EQ(result.width, expectedDimension(setup.imageSizeX, setup.sizeX, setup.shiftX, setup.border))
            << "imageSizeX: " << setup.imageSizeX << " sizeX: " << setup.sizeX << " shiftX: " << setup.shiftX
            << " border " << setup.border;
        ASSERT_EQ(result.height, expectedDimension(setup.imageSizeY, setup.sizeY, setup.shiftY, setup.border))
            << "imageSizeY: " << setup.imageSizeY << " sizeY: " << setup.sizeY << " shiftY: " << setup.shiftY
            << " border " << setup.border;
        EXPECT_TRUE(transformationData.parametersCalled);
    }
}

// test transform
// test transforming mutliple images with changing sizes

} // namespace Terrahertz::UnitTests
