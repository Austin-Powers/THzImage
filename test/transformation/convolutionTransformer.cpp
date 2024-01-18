#include "THzImage/transformation/convolutionTransformer.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/common/pixel.hpp"
#include "THzImage/io/testImageGenerator.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

namespace Terrahertz::UnitTests {

struct TransformationLineBuffer : public testing::Test
{
    void SetUp() override
    {
        EXPECT_TRUE(image.setDimensions(imageDimensions));
        mirror.resize(bufferDimensions.area());
        for (auto i = 0U; i < imageDimensions.area(); ++i)
        {
            auto const i8 = static_cast<std::uint8_t>(i);
            image[i]      = BGRAPixel{i8, i8, i8, i8};
        }
    }

    void readNextLineAndCompare(std::uint32_t const imageIdx, std::uint32_t const bufferIdx) noexcept
    {
        for (auto i = 0U; i < mirror.size(); ++i)
        {
            mirror[i] = sut.data()[i];
        }
        EXPECT_TRUE(sut.readNextLine(view));
        for (auto y = 0U; y < bufferDimensions.height; ++y)
        {
            if (y != bufferIdx)
            {
                for (auto x = 0U; x < bufferDimensions.width; ++x)
                {
                    auto const idx = x + (y * bufferDimensions.width);
                    EXPECT_EQ(mirror[idx], sut.data()[idx]);
                }
            }
            else
            {
                for (auto x = 0U; x < bufferDimensions.width; ++x)
                {
                    auto const bufIdx = x + (y * bufferDimensions.width);
                    auto const imgIdx = x + (imageIdx * imageDimensions.width);
                    EXPECT_EQ(sut.data()[bufIdx], image[imgIdx]);
                }
            }
        }
    }

    Rectangle const imageDimensions{5U, 7U};

    Rectangle const bufferDimensions{3U, 3U};

    BGRAImage image{};

    BGRAImageView view{};

    Internal::LineBuffer<BGRAPixel> sut{};

    std::vector<BGRAPixel> mirror{};
};

TEST_F(TransformationLineBuffer, GeneralOperation)
{
    view = image.view();
    sut.setup(bufferDimensions.area(), bufferDimensions.width, imageDimensions.width - bufferDimensions.width);
    readNextLineAndCompare(0U, 0U);
    readNextLineAndCompare(1U, 1U);
    readNextLineAndCompare(2U, 2U);
    readNextLineAndCompare(3U, 0U);
    readNextLineAndCompare(4U, 1U);
    readNextLineAndCompare(5U, 2U);
    readNextLineAndCompare(6U, 0U);
    EXPECT_FALSE(sut.readNextLine(view));
    view = image.view();
    sut.setup(bufferDimensions.area(), bufferDimensions.width, imageDimensions.width - bufferDimensions.width);
    readNextLineAndCompare(0U, 0U);
    readNextLineAndCompare(1U, 1U);
    readNextLineAndCompare(2U, 2U);
    readNextLineAndCompare(3U, 0U);
    readNextLineAndCompare(4U, 1U);
    readNextLineAndCompare(5U, 2U);
    readNextLineAndCompare(6U, 0U);
    EXPECT_FALSE(sut.readNextLine(view));
}

TEST_F(TransformationLineBuffer, ResettingAfterTransformationError)
{
    view = image.view();
    sut.setup(bufferDimensions.area(), bufferDimensions.width, imageDimensions.width - bufferDimensions.width);
    readNextLineAndCompare(0U, 0U);
    readNextLineAndCompare(1U, 1U);
    readNextLineAndCompare(2U, 2U);
    readNextLineAndCompare(3U, 0U);
    readNextLineAndCompare(4U, 1U);
    readNextLineAndCompare(5U, 2U);
    EXPECT_TRUE(view.skip()); // throw a spanner in the works by getting buffer and view out of lockstep
    EXPECT_FALSE(sut.readNextLine(view));
    view = image.view();
    sut.setup(bufferDimensions.area(), bufferDimensions.width, imageDimensions.width - bufferDimensions.width);
    readNextLineAndCompare(0U, 0U);
    readNextLineAndCompare(1U, 1U);
    readNextLineAndCompare(2U, 2U);
    readNextLineAndCompare(3U, 0U);
    readNextLineAndCompare(4U, 1U);
    readNextLineAndCompare(5U, 2U);
    readNextLineAndCompare(6U, 0U);
    EXPECT_FALSE(sut.readNextLine(view));
}

struct MatrixHelper : public testing::TestWithParam<std::uint32_t>
{
    void SetUp() override
    {
        buffer.setup(bufferDimensions.area(), bufferDimensions.width, 0U);
        sut.setup(buffer.data(), bufferDimensions.width, bufferDimensions.height, matrixWidth, matrixShift);
    }

    void exhaustNextCalls() noexcept
    {
        for (auto i = 0U; i < bufferDimensions.width; ++i)
        {
            if (!sut.next())
            {
                break;
            }
        }
    }

    void mirrorMatrix() noexcept
    {
        auto const matrix = sut();
        mirror.clear();
        for (auto i = 0U; i < bufferDimensions.height; ++i)
        {
            mirror.push_back(matrix[i]);
        }
    }

    std::uint32_t stepsPerLine() noexcept { return (bufferDimensions.width - matrixWidth) / matrixShift; }

    Rectangle const bufferDimensions{8U, 4U};

    std::uint32_t matrixWidth{3U};

    std::uint32_t matrixShift{GetParam()};

    Internal::LineBuffer<BGRAPixel> buffer{};

    Internal::MatrixHelper<BGRAPixel> sut{};

    std::vector<BGRAPixel const *> mirror{};
};

TEST_P(MatrixHelper, SetupCorrect)
{
    auto const matrix = sut();

    auto bufferPtr = buffer.data();
    for (auto i = 0U; i < bufferDimensions.height; ++i)
    {
        EXPECT_EQ(matrix[i], bufferPtr);
        bufferPtr += bufferDimensions.width;
    }
}

TEST_P(MatrixHelper, AccessingMatrixContent) { ASSERT_EQ(sut()[0U][0U], BGRAPixel{}); }

TEST_P(MatrixHelper, NextReturnsFalseAfterLineIsExhausted)
{
    for (auto i = 0U; i < stepsPerLine(); ++i)
    {
        EXPECT_TRUE(sut.next());
    }
    EXPECT_FALSE(sut.next());
    EXPECT_FALSE(sut.next());
    EXPECT_FALSE(sut.next());
}

TEST_P(MatrixHelper, NextMovesPointersAsExpected)
{
    mirrorMatrix();
    for (auto i = 0U; i < stepsPerLine(); ++i)
    {
        EXPECT_TRUE(sut.next());
        auto matrix = sut();
        for (auto i = 0U; i < mirror.size(); ++i)
        {
            mirror[i] += matrixShift;
            EXPECT_EQ(matrix[i], mirror[i]);
        }
    }
}

TEST_P(MatrixHelper, NextReturnsTrueAfterCallToLineFeed)
{
    exhaustNextCalls();
    sut.lineFeed(1U);
    EXPECT_TRUE(sut.next());
}

TEST_P(MatrixHelper, LineFeedShiftsPointersAsExpected)
{
    for (auto line = 0U; line < 3U; ++line)
    {
        mirrorMatrix();
        exhaustNextCalls();
        sut.lineFeed(0U);
        auto matrix = sut();
        for (auto i = 0U; i < mirror.size(); ++i)
        {
            EXPECT_EQ(matrix[i], mirror[(i + 1U) % mirror.size()]) << "Line: " << line;
        }
    }
}

TEST_P(MatrixHelper, LineFeedShiftsPointersAsExpectedWithLinesToSkip)
{
    for (auto line = 0U; line < 3U; ++line)
    {
        mirrorMatrix();
        exhaustNextCalls();
        sut.lineFeed(2U);
        auto matrix = sut();
        for (auto i = 0U; i < mirror.size(); ++i)
        {
            EXPECT_EQ(matrix[i], mirror[(i + 3U) % mirror.size()]) << "Line: " << line;
        }
    }
}

TEST_P(MatrixHelper, CallingSetupResetsHelper)
{
    exhaustNextCalls();
    sut.setup(buffer.data(), bufferDimensions.width, bufferDimensions.height, matrixWidth, matrixShift);
    mirrorMatrix();
    for (auto i = 0U; i < stepsPerLine(); ++i)
    {
        EXPECT_TRUE(sut.next());
        auto matrix = sut();
        for (auto j = 0U; j < mirror.size(); ++j)
        {
            mirror[j] += matrixShift;
            EXPECT_EQ(matrix[j], mirror[j]);
        }
    }
}

TEST_P(MatrixHelper, UsingMatrixOfExhaustedHelperDoesNotCauseAnError)
{
    // through this behavior we can omit any checks in the transform method of the ConvolutionTransformer
    exhaustNextCalls();
    for (auto y = 0U; y < bufferDimensions.height; ++y)
    {
        for (auto x = 0U; x < matrixWidth; ++x)
        {
            EXPECT_EQ(sut()[y][x], BGRAPixel{});
        }
    }
}

INSTANTIATE_TEST_SUITE_P(TransformationMatrixHelper, MatrixHelper, testing::Values(1U, 2U, 3U));

struct TransformationConvolutionTransformer : public testing::Test
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
            ConvolutionParameters parameters{1U, 1U, 1U, 1U};

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

    using TestClass = ConvolutionTransformer<BGRAPixel, MockTransformation>;

    void SetUp() override { transformation.data = &transformationData; }

    bool nextScenario(Scenario &s) const noexcept
    {
        ++s.imageX;
        if (s.imageX >= 6U)
        {
            s.imageX = 1U;
            ++s.imageY;
        }
        if (s.imageY >= 6U)
        {
            s.imageY = 1U;
            ++s.matrixX;
        }
        if (s.matrixX >= 5U)
        {
            s.matrixX = 1U;
            ++s.matrixY;
        }
        if (s.matrixY >= 5U)
        {
            s.matrixY = 1U;
            ++s.shiftX;
        }
        if (s.shiftX >= 3U)
        {
            s.shiftX = 1U;
            ++s.shiftY;
        }
        return s.shiftY < 3U;
    }

    ConvolutionParameters toParameters(Scenario const &s) const noexcept
    {
        return ConvolutionParameters{s.matrixX, s.matrixY, s.shiftX, s.shiftY};
    }

    bool checkAndPrint(bool const fail, Scenario scenario) noexcept
    {
        if (fail)
        {
            std::cout << "\n=== Scenario ===";
            std::cout << "\nimageX:  " << scenario.imageX;
            std::cout << "\nimageY:  " << scenario.imageY;
            std::cout << "\nmatrixX: " << scenario.matrixX;
            std::cout << "\nmatrixY: " << scenario.matrixY;
            std::cout << "\nshiftX:  " << scenario.shiftX;
            std::cout << "\nshiftY:  " << scenario.shiftY;
            std::cout << "\n===   End    ===\n" << std::endl;
        }
        return fail;
    };

    BGRAPixel pixel{};

    MockTransformer mockTransformer{};

    MockTransformation::Data transformationData{};

    MockTransformation transformation{};
};

TEST_F(TransformationConvolutionTransformer, ParameterConstructionThrowsIfGivenInvalidValues)
{
    EXPECT_THROW(ConvolutionParameters wrongSizeX(0U, 1U, 1U, 1U), std::invalid_argument);
    EXPECT_THROW(ConvolutionParameters wrongSizeY(1U, 0U, 1U, 1U), std::invalid_argument);
    EXPECT_THROW(ConvolutionParameters wrongShiftX(1U, 1U, 0U, 1U), std::invalid_argument);
    EXPECT_THROW(ConvolutionParameters wrongShiftY(1U, 1U, 1U, 0U), std::invalid_argument);
}

TEST_F(TransformationConvolutionTransformer, ParametersReturnCorrectValues)
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

TEST_F(TransformationConvolutionTransformer, ParametersRequestedByTransformer)
{
    transformationData.parametersCalled = false;
    EXPECT_CALL(mockTransformer, dimensions()).Times(1).WillOnce(testing::Return(Rectangle{2U, 2U}));
    EXPECT_CALL(mockTransformer, transform(pixel)).Times(2).WillRepeatedly(testing::Return(true));
    TestClass sut{mockTransformer, transformation};
    EXPECT_TRUE(transformationData.parametersCalled);
}

TEST_F(TransformationConvolutionTransformer, ResetRelayedCorrectly)
{
    // once on construction, once on reset
    EXPECT_CALL(mockTransformer, dimensions()).Times(2).WillRepeatedly(testing::Return(Rectangle{2U, 2U}));
    EXPECT_CALL(mockTransformer, transform(pixel)).Times(4).WillRepeatedly(testing::Return(true));
    TestClass sut{mockTransformer, transformation};
    EXPECT_CALL(mockTransformer, reset())
        .Times(2)
        .WillOnce(testing::Return(true))
        .WillRepeatedly(testing::Return(false));
    EXPECT_TRUE(sut.reset());
    EXPECT_FALSE(sut.reset());
}

TEST_F(TransformationConvolutionTransformer, NextImageRelayedCorrectly)
{
    // once on construction, once on nextImage
    EXPECT_CALL(mockTransformer, dimensions()).Times(2).WillRepeatedly(testing::Return(Rectangle{2U, 2U}));
    EXPECT_CALL(mockTransformer, transform(pixel)).Times(4).WillRepeatedly(testing::Return(true));
    TestClass sut{mockTransformer, transformation};
    EXPECT_CALL(mockTransformer, nextImage())
        .Times(2)
        .WillOnce(testing::Return(true))
        .WillRepeatedly(testing::Return(false));
    EXPECT_TRUE(sut.nextImage());
    EXPECT_FALSE(sut.nextImage());
}

TEST_F(TransformationConvolutionTransformer, DimensionsAndSetupCallsToBaseCorrect)
{
    struct CallPatternAwareMockTransformer : public IImageTransformer<BGRAPixel>
    {
        Rectangle dimensions() const noexcept override
        {
            if (finalVerdict)
            {
                finalVerdict = (stage == Stage::Dimensions);
                stage        = Stage::Transform;
                currentCalls = transformCalls;
                if (!finalVerdict)
                {
                    std::cout << "dimensions() called at the wrong stage\n";
                }
            }
            return _dimensions;
        }

        bool transform(BGRAPixel &pixel) noexcept override
        {
            if (finalVerdict)
            {
                finalVerdict = (stage == Stage::Transform);
                --currentCalls;
                if (currentCalls == 0U)
                {
                    if (skipCalls != 0U)
                    {
                        stage        = Stage::Skip;
                        currentCalls = skipCalls;
                    }
                    else
                    {
                        currentCalls = transformCalls;
                    }
                }
                if (!finalVerdict)
                {
                    std::cout << "transform() called at the wrong stage\n";
                }
            }
            return finalVerdict;
        }

        bool skip() noexcept override
        {
            if (finalVerdict)
            {
                finalVerdict = (stage == Stage::Skip);
                --currentCalls;
                if (currentCalls == 0U)
                {
                    --remainingLines;
                    if (remainingLines == 0U)
                    {
                        stage = Stage::Finish;
                    }
                    else
                    {
                        stage        = Stage::Transform;
                        currentCalls = transformCalls;
                    }
                }
                if (!finalVerdict)
                {
                    std::cout << "skip() called at the wrong stage\n";
                }
            }
            return finalVerdict;
        }

        bool reset() noexcept override
        {
            finalVerdict = false;
            std::cout << "reset() called\n";
            return finalVerdict;
        }

        bool nextImage() noexcept override
        {
            finalVerdict = false;
            std::cout << "nextImage() called\n";
            return finalVerdict;
        }

        void setup(Scenario const &scenario) noexcept
        {
            auto const expectedValue = [](std::uint16_t const image,
                                          std::uint16_t const matrix,
                                          std::uint16_t const shift) noexcept -> std::uint16_t {
                if (image < matrix)
                {
                    return 0U;
                }
                return ((image - matrix) / shift) + 1U;
            };

            _dimensions    = Rectangle{scenario.imageX, scenario.imageY};
            expectedWidth  = expectedValue(scenario.imageX, scenario.matrixX, scenario.shiftX);
            expectedHeight = expectedValue(scenario.imageY, scenario.matrixY, scenario.shiftY);
            stage          = Stage::Dimensions;
            finalVerdict   = true;
            transformCalls = scenario.matrixX + ((expectedWidth - 1U) * scenario.shiftX);
            skipCalls      = scenario.imageX - transformCalls;
            remainingLines = scenario.matrixY;
        }

        bool callsReceivedAsExpected() noexcept { return finalVerdict; }

        Rectangle _dimensions{};

        std::uint32_t expectedWidth{};

        std::uint32_t expectedHeight{};

        enum class Stage
        {
            Dimensions,
            Transform,
            Skip,
            Finish
        } mutable stage{};

        bool mutable finalVerdict{};

        std::uint32_t transformCalls{};

        std::uint32_t skipCalls{};

        std::uint32_t mutable currentCalls{};

        std::uint32_t remainingLines{};

    } myBaseTransformer;

    Scenario scenario{};
    scenario.imageY  = 2U;
    scenario.matrixY = 2U;
    do
    {
        myBaseTransformer.setup(scenario);
        transformationData.parameters = toParameters(scenario);
        TestClass sut{myBaseTransformer, transformation};
        ASSERT_FALSE(checkAndPrint(!myBaseTransformer.callsReceivedAsExpected(), scenario));

        auto const dimensions = sut.dimensions();
        ASSERT_FALSE(checkAndPrint(dimensions.width != myBaseTransformer.expectedWidth, scenario));
        ASSERT_FALSE(checkAndPrint(dimensions.height != myBaseTransformer.expectedHeight, scenario));
    } while (nextScenario(scenario));
}

TEST_F(TransformationConvolutionTransformer, DISABLED_TransformAndSkip)
{
    BGRAImage     image{};
    BGRAImageView base{};
    Scenario      scenario{};
    BGRAPixel     transformationResult{};

    auto const setupLambda = [&]() noexcept {
        TestImageGenerator generator{Rectangle{scenario.imageX, scenario.imageY}};
        EXPECT_TRUE(image.read(generator));
        transformationData.parameters = toParameters(scenario);

        base = image.view();
    };

    do
    {
        if (scenario.imageY > 1U)
        {
            printf("here\n");
        }
        setupLambda();
        TestClass  sut{base, transformation};
        auto const pixelCount = sut.dimensions().area();

        bool skip{};
        for (auto i = 0U; i < pixelCount; ++i)
        {
            if (skip)
            {
                ASSERT_FALSE(checkAndPrint(!sut.skip(), scenario));
            }
            else
            {
                ASSERT_FALSE(checkAndPrint(!sut.transform(transformationResult), scenario));
            }
            skip = !skip;
        }
        ASSERT_FALSE(checkAndPrint(sut.transform(transformationResult), scenario));
        ASSERT_FALSE(checkAndPrint(sut.skip(), scenario));
    } while (nextScenario(scenario));
}

} // namespace Terrahertz::UnitTests
