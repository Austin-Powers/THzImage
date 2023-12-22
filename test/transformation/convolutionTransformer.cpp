#include "THzImage/transformation/convolutionTransformer.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/common/pixel.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <array>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

namespace Terrahertz::UnitTests {

struct Transformation_ConvolutionTransformerBasics : public testing::Test
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

        BGRAPixel operator()(BGRAPixel const **const matrix) noexcept { return matrix[0U][0U]; }

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

    std::uint32_t expectedDimension(std::uint16_t const image,
                                    std::uint16_t const matrix,
                                    std::uint16_t const stepSize,
                                    bool const          border) noexcept
    {
        if (stepSize == 1U)
        {
            if (border)
            {
                return image;
            }
            return image - matrix + 1U;
        }

        std::uint32_t result{};
        for (auto start = 0U; start < image; start += stepSize)
        {
            if (!border)
            {
                for (auto pos = 0U; pos < matrix; ++pos)
                {
                    if ((start + pos) >= image)
                    {
                        return result;
                    }
                }
            }
            ++result;
        }
        return result;
    };
};

TEST_F(Transformation_ConvolutionTransformerBasics, ParameterConstructionThrowIfGivenInvalidValues)
{
    BGRAPixel const pixel{};
    EXPECT_THROW(BGRACParameters wrongSizeX(0U, 1U, 1U, 1U, false, pixel), std::invalid_argument);
    EXPECT_THROW(BGRACParameters wrongSizeX(1U, 0U, 1U, 1U, false, pixel), std::invalid_argument);
    EXPECT_THROW(BGRACParameters wrongSizeX(1U, 1U, 0U, 1U, false, pixel), std::invalid_argument);
    EXPECT_THROW(BGRACParameters wrongSizeX(1U, 1U, 1U, 0U, false, pixel), std::invalid_argument);
}

TEST_F(Transformation_ConvolutionTransformerBasics, ParametersReturnCorrectValues)
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

TEST_F(Transformation_ConvolutionTransformerBasics, NextImageIsPassedThroughCorrectly)
{
    Rectangle const testDimensions{160U, 128U};
    // called once construction and when next image is successful
    EXPECT_CALL(baseTransformer, dimensions()).Times(2).WillRepeatedly(testing::Return(testDimensions));
    EXPECT_CALL(baseTransformer, nextImage()).Times(2).WillOnce(testing::Return(true)).WillOnce(testing::Return(false));
    ClassUnderTest sut{baseTransformer, transformation};
    EXPECT_TRUE(sut.nextImage());
    EXPECT_FALSE(sut.nextImage());
}

TEST_F(Transformation_ConvolutionTransformerBasics, ResetIsPassedThroughCorrectly)
{
    Rectangle const testDimensions{160U, 128U};
    // called once construction and when next image is successful
    EXPECT_CALL(baseTransformer, dimensions()).Times(2).WillRepeatedly(testing::Return(testDimensions));
    EXPECT_CALL(baseTransformer, reset()).Times(2).WillOnce(testing::Return(true)).WillOnce(testing::Return(false));
    ClassUnderTest sut{baseTransformer, transformation};
    EXPECT_TRUE(sut.reset());
    EXPECT_FALSE(sut.reset());
}

TEST_F(Transformation_ConvolutionTransformerBasics, DimensionsReturnsExpectedResults)
{
    auto errorCount = 0U;
    for (auto const &setup : setups)
    {
        transformationData.parametersCalled = false;
        transformationData.parameters =
            BGRACParameters{setup.sizeX, setup.sizeY, setup.shiftX, setup.shiftY, setup.border, borderFill};
        Rectangle imageDimensions{setup.imageSizeX, setup.imageSizeY};
        EXPECT_CALL(baseTransformer, dimensions()).Times(1).WillOnce(testing::Return(imageDimensions));
        ClassUnderTest sut{baseTransformer, transformation};

        auto const result         = sut.dimensions();
        auto const expectedWidth  = expectedDimension(setup.imageSizeX, setup.sizeX, setup.shiftX, setup.border);
        auto const expectedHeight = expectedDimension(setup.imageSizeY, setup.sizeY, setup.shiftY, setup.border);
        if (result.width != expectedWidth)
        {
            ++errorCount;
        }
        if (result.height != expectedHeight)
        {
            ++errorCount;
        }

        EXPECT_EQ(result.width, expectedWidth) << "imageSizeX: " << setup.imageSizeX << " sizeX: " << setup.sizeX
                                               << " shiftX: " << setup.shiftX << " border " << setup.border;
        EXPECT_EQ(result.height, expectedHeight) << "imageSizeY: " << setup.imageSizeY << " sizeY: " << setup.sizeY
                                                 << " shiftY: " << setup.shiftY << " border " << setup.border;
        EXPECT_TRUE(transformationData.parametersCalled);
    }
    EXPECT_EQ(errorCount, 0U);
}

struct Transformation_ConvolutionTransformerTransformation : public testing::Test
{
    static constexpr BGRAPixel const BorderColor{0x0U, 0x0U, 0x0U, 0xFFU};

    template <std::uint16_t TWidth, std::uint16_t THeight, std::uint16_t TShiftX, std::uint16_t TShiftY, bool TBorder>
    struct TransformationTemplate
    {
        ConvolutionParameters<BGRAPixel> parameters() const noexcept
        {
            return ConvolutionParameters<BGRAPixel>{TWidth, THeight, TShiftX, TShiftY, TBorder, BorderColor};
        }

        BGRAPixel operator()(BGRAPixel const **const matrix)
        {
            std::uint32_t r{};
            std::uint32_t g{};
            std::uint32_t b{};
            for (auto y = 0U; y < THeight; ++y)
            {
                for (auto x = 0U; x < TWidth; ++x)
                {
                    b += matrix[y][x].blue;
                    g += matrix[y][x].green;
                    r += matrix[y][x].red;
                }
            }
            auto const area = TWidth * THeight;
            return BGRAPixel{static_cast<std::uint8_t>(b / area),
                             static_cast<std::uint8_t>(g / area),
                             static_cast<std::uint8_t>(r / area)};
        }
    };

    void SetUp() override
    {
        TestImageGenerator generator{Rectangle{12U, 12U}};
        EXPECT_TRUE(image.read(generator));
        view = image.view();
    }

    std::vector<BGRAPixel> prepareData(std::uint16_t const width, std::uint16_t const height) const noexcept
    {
        auto const xBorderSize  = (width - 1U);
        auto const yBorderSize  = (height - 1U);
        auto const leftBorder   = xBorderSize / 2U;
        auto const topBorder    = yBorderSize / 2U;
        auto const rightBorder  = xBorderSize - leftBorder;
        auto const bottomBorder = yBorderSize - topBorder;

        auto const dataWidth  = image.dimensions().width + leftBorder + rightBorder;
        auto const dataHeight = image.dimensions().height + topBorder + bottomBorder;

        auto myView = image.view();

        std::vector<BGRAPixel> data{};

        auto const addEmptyLine = [&]() noexcept -> void {
            for (auto i = 0U; i < dataWidth; ++i)
            {
                data.push_back(BorderColor);
            }
        };
        auto const addRegularLine = [&]() noexcept -> void {
            for (auto i = 0U; i < leftBorder; ++i)
            {
                data.push_back(BorderColor);
            }
            for (auto i = 0U; i < image.dimensions().width; ++i)
            {
                data.push_back(*myView);
                ++myView;
            }
            for (auto i = 0U; i < rightBorder; ++i)
            {
                data.push_back(BorderColor);
            }
        };

        for (auto i = 0U; i < topBorder; ++i)
        {
            addEmptyLine();
        }
        for (auto i = 0U; i < image.dimensions().height; ++i)
        {
            addRegularLine();
        }
        for (auto i = 0U; i < bottomBorder; ++i)
        {
            addEmptyLine();
        }
        return data;
    };

    Image<BGRAPixel>     image{};
    ImageView<BGRAPixel> view{};
};

TEST_F(Transformation_ConvolutionTransformerTransformation, BorderTrueAndShiftOne)
{
    using TestTransformation = TransformationTemplate<3U, 3U, 1U, 1U, true>;

    TestTransformation transformation{};

    ConvolutionTransformer<BGRAPixel, TestTransformation> transformer{view, transformation};
    EXPECT_EQ(view.dimensions(), transformer.dimensions());

    auto const params = transformation.parameters();
    auto const data   = prepareData(params.sizeX(), params.sizeY());

    std::array<BGRAPixel const *, 3U> lines{};

    auto const totalLineWidth = image.dimensions().width + params.sizeX() - 1U;

    lines[0U] = &data[0U];
    lines[1U] = &data[totalLineWidth];
    lines[2U] = &data[totalLineWidth * 2U];

    BGRAPixel pixel;
    // EXPECT_TRUE(transformer.transform(pixel));
    // EXPECT_EQ(transformation(lines.data()), pixel);
}

// test transforming mutliple images with changing sizes

} // namespace Terrahertz::UnitTests
