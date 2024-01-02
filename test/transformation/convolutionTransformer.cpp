#include "THzImage/transformation/convolutionTransformer.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/common/pixel.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct Transformation_ConvolutionTransformer : public testing::Test
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

    class MockTransformation
    {
    public:
        struct Data
        {
            ConvolutionParameters parameters{3U, 3U, 1U, 1U};

            bool parametersCalled{};
        };

        ConvolutionParameters parameters() noexcept
        {
            data->parametersCalled = true;
            return data->parameters;
        }

        BGRAPixel operator()(BGRAPixel const **const matrix) noexcept { return matrix[0U][0U]; }

        Data *data{};
    };

    struct Scenario
    {
        std::uint16_t imageX{1U};
        std::uint16_t imageY{1U};
        std::uint16_t matrixX{1U};
        std::uint16_t matrixY{1U};
        std::uint16_t shiftX{1U};
        std::uint16_t shiftY{1U};
    };

    MockTransformer mockTransformer{};

    MockTransformation::Data transformationData{};

    MockTransformation transformation{};

    void SetUp() override { transformation.data = &transformationData; }

    bool nextScenario(Scenario &s) const noexcept
    {
        ++s.imageX;
        if (s.imageX >= 8U)
        {
            s.imageX = 1U;
            ++s.imageY;
        }
        if (s.imageY >= 8U)
        {
            s.imageY = 1U;
            ++s.matrixX;
        }
        if (s.matrixX >= 6U)
        {
            s.matrixX = 1U;
            ++s.matrixY;
        }
        if (s.matrixY >= 6U)
        {
            s.matrixY = 1U;
            ++s.shiftX;
        }
        if (s.shiftX >= 4U)
        {
            s.shiftX = 1U;
            ++s.shiftY;
        }
        return s.shiftY < 4U;
    }

    ConvolutionParameters toParameters(Scenario const &s) const noexcept
    {
        return ConvolutionParameters{s.matrixX, s.matrixY, s.shiftX, s.shiftY};
    }
};

TEST_F(Transformation_ConvolutionTransformer, ParameterConstructionThrowIfGivenInvalidValues)
{
    EXPECT_THROW(ConvolutionParameters wrongSizeX(0U, 1U, 1U, 1U), std::invalid_argument);
    EXPECT_THROW(ConvolutionParameters wrongSizeY(1U, 0U, 1U, 1U), std::invalid_argument);
    EXPECT_THROW(ConvolutionParameters wrongShiftX(1U, 1U, 0U, 1U), std::invalid_argument);
    EXPECT_THROW(ConvolutionParameters wrongShiftY(1U, 1U, 1U, 0U), std::invalid_argument);
}

TEST_F(Transformation_ConvolutionTransformer, ParametersReturnCorrectValues)
{
    std::uint16_t const sizeX{7U};
    std::uint16_t const sizeY{5U};
    std::uint16_t const shiftX{2U};
    std::uint16_t const shiftY{1U};

    ConvolutionParameters const parameters{sizeX, sizeY, shiftX, shiftY};
    EXPECT_EQ(parameters.sizeX(), sizeX);
    EXPECT_EQ(parameters.sizeY(), sizeY);
    EXPECT_EQ(parameters.shiftX(), shiftX);
    EXPECT_EQ(parameters.shiftY(), shiftY);
}

TEST_F(Transformation_ConvolutionTransformer, ParametersRequestedByTransformer)
{
    transformationData.parametersCalled = false;
    EXPECT_CALL(mockTransformer, dimensions()).Times(1).WillOnce(testing::Return(Rectangle{2U, 2U}));
    ConvolutionTransformer<BGRAPixel, MockTransformation> sut{mockTransformer, transformation};
    EXPECT_TRUE(transformationData.parametersCalled);
}

TEST_F(Transformation_ConvolutionTransformer, ResetRelayedCorrectly)
{
    // once on construction, once on reset
    EXPECT_CALL(mockTransformer, dimensions()).Times(2).WillRepeatedly(testing::Return(Rectangle{2U, 2U}));
    ConvolutionTransformer<BGRAPixel, MockTransformation> sut{mockTransformer, transformation};
    EXPECT_CALL(mockTransformer, reset())
        .Times(2)
        .WillOnce(testing::Return(true))
        .WillRepeatedly(testing::Return(false));
    EXPECT_TRUE(sut.reset());
    EXPECT_FALSE(sut.reset());
}

TEST_F(Transformation_ConvolutionTransformer, NextImageRelayedCorrectly)
{
    // once on construction, once on nextImage
    EXPECT_CALL(mockTransformer, dimensions()).Times(2).WillRepeatedly(testing::Return(Rectangle{2U, 2U}));
    ConvolutionTransformer<BGRAPixel, MockTransformation> sut{mockTransformer, transformation};
    EXPECT_CALL(mockTransformer, nextImage())
        .Times(2)
        .WillOnce(testing::Return(true))
        .WillRepeatedly(testing::Return(false));
    EXPECT_TRUE(sut.nextImage());
    EXPECT_FALSE(sut.nextImage());
}

TEST_F(Transformation_ConvolutionTransformer, DimensionsCorrect)
{
    auto const expectedValue =
        [](std::uint16_t const image, std::uint16_t const matrix, std::uint16_t const shift) noexcept -> std::uint16_t {
        if (image < matrix)
        {
            return 0U;
        }
        return ((image - matrix) / shift) + 1U;
    };
    Scenario scenario{};
    do
    {
        EXPECT_CALL(mockTransformer, dimensions())
            .Times(1)
            .WillOnce(testing::Return(Rectangle{scenario.imageX, scenario.imageY}));
        transformationData.parameters = toParameters(scenario);
        ConvolutionTransformer<BGRAPixel, MockTransformation> sut{mockTransformer, transformation};

        auto const dimensions = sut.dimensions();
        ASSERT_EQ(dimensions.width, expectedValue(scenario.imageX, scenario.matrixX, scenario.shiftX))
            << "image: " << scenario.imageX << " matrix " << scenario.matrixX << " shift " << scenario.shiftX;
        ASSERT_EQ(dimensions.height, expectedValue(scenario.imageY, scenario.matrixY, scenario.shiftY))
            << "image: " << scenario.imageY << " matrix " << scenario.matrixY << " shift " << scenario.shiftY;
    } while (nextScenario(scenario));
}

} // namespace Terrahertz::UnitTests
