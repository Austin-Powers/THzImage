#include "THzImage/transformation/convolutionTransformerBase.hpp"

#include "THzImage/common/image.hpp"
#include "THzImage/common/pixel.hpp"
#include "THzImage/io/testImageGenerator.hpp"
#include "mockTransformer.hpp"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct ConvolutionTransformerBaseTests : public testing::Test
{
    using CallType = MockTransformer<BGRAPixel>::CallType;

    class TestClass : public ConvolutionTransformerBase<BGRAPixel>
    {
    public:
        TestClass(IImageTransformer<BGRAPixel> &base) noexcept : ConvolutionTransformerBase<BGRAPixel>{base} {}

        void getParameters(std::uint32_t &matrixWidth,
                           std::uint32_t &matrixHeight,
                           std::uint32_t &matrixShiftX,
                           std::uint32_t &matrixShiftY) noexcept override
        {
            ++getParametersCallCount;
            matrixWidth  = matrixWidthValue;
            matrixHeight = matrixHeightValue;
            matrixShiftX = matrixShiftXValue;
            matrixShiftY = matrixShiftYValue;
        }

        BGRAPixel convolute(BGRAPixel const **const matrix) noexcept override
        {
            BGRAPixel32 result{};
            for (auto y = 0U; y < matrixHeightValue; ++y)
            {
                for (auto x = 0U; x < matrixWidthValue; ++x)
                {
                    result += matrix[y][x];
                }
            }
            result /= matrixWidthValue * matrixHeightValue;
            return result;
        }

        std::uint32_t matrixWidthValue{3U};

        std::uint32_t matrixHeightValue{3U};

        std::uint32_t matrixShiftXValue{1U};

        std::uint32_t matrixShiftYValue{1U};

        std::uint32_t getParametersCallCount{};
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

    Rectangle expectedDimensions(Scenario &s) noexcept
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
        return Rectangle{expectedValue(s.imageX, s.matrixX, s.shiftX), expectedValue(s.imageY, s.matrixY, s.shiftY)};
    }

    MockTransformer<BGRAPixel> mockTransformer{};
};

TEST_F(ConvolutionTransformerBaseTests, DimensionsAreaZeroAfterConstruction)
{
    // We can not call getParameters from the constructor of the base class,
    // as this would crash the program due to the derived implementing the method not being there
    // because dimensions() is const we also cannot call setup there
    // this should not be a problem as Image<> is expexted to call reset() at the start of executeAndIngest
    TestClass sut{mockTransformer};
    EXPECT_EQ(sut.dimensions(), Rectangle{});
}

TEST_F(ConvolutionTransformerBaseTests, GetParametersCalled)
{
    TestClass sut{mockTransformer};
    EXPECT_EQ(sut.getParametersCallCount, 0U);
    // getParameters should not be called if reset of the wrapped returns false
    EXPECT_FALSE(sut.reset());
    EXPECT_EQ(sut.getParametersCallCount, 0U);
    // getParameters should not be called if nextImage of the wrapped returns false
    EXPECT_FALSE(sut.nextImage());
    EXPECT_EQ(sut.getParametersCallCount, 0U);

    mockTransformer.resetReturnValue = true;
    EXPECT_TRUE(sut.reset());
    EXPECT_EQ(sut.getParametersCallCount, 1U);
    mockTransformer.nextImageReturnValue = true;
    EXPECT_TRUE(sut.nextImage());
    EXPECT_EQ(sut.getParametersCallCount, 2U);
}

TEST_F(ConvolutionTransformerBaseTests, ResetPassedThrough)
{
    TestClass sut{mockTransformer};
    EXPECT_FALSE(sut.reset());
    mockTransformer.resetReturnValue = true;
    EXPECT_TRUE(sut.reset());
    EXPECT_EQ(mockTransformer.countCalls(CallType::Reset), 2U);
    EXPECT_EQ(mockTransformer.countCalls(CallType::Transform), 0U);
    EXPECT_EQ(mockTransformer.countCalls(CallType::Skip), 0U);
}

TEST_F(ConvolutionTransformerBaseTests, NextImagePassedThrough)
{
    TestClass sut{mockTransformer};
    EXPECT_FALSE(sut.nextImage());
    mockTransformer.nextImageReturnValue = true;
    EXPECT_TRUE(sut.nextImage());
    EXPECT_EQ(mockTransformer.countCalls(CallType::NextImage), 2U);
    EXPECT_EQ(mockTransformer.countCalls(CallType::Transform), 0U);
    EXPECT_EQ(mockTransformer.countCalls(CallType::Skip), 0U);
}

TEST_F(ConvolutionTransformerBaseTests, ResetReturnsFalseIfParametersContainZero)
{
    mockTransformer.resetReturnValue = true;
    TestClass sut{mockTransformer};
    sut.matrixWidthValue = 0U;
    EXPECT_FALSE(sut.reset());
    sut.matrixWidthValue  = 3U;
    sut.matrixHeightValue = 0U;
    EXPECT_FALSE(sut.reset());
    sut.matrixHeightValue = 3U;
    sut.matrixShiftXValue = 0U;
    EXPECT_FALSE(sut.reset());
    sut.matrixShiftXValue = 1U;
    sut.matrixShiftYValue = 0U;
    EXPECT_FALSE(sut.reset());
    sut.matrixShiftYValue = 1U;
    EXPECT_TRUE(sut.reset());
    EXPECT_EQ(mockTransformer.countCalls(CallType::Reset), 5U);
}

TEST_F(ConvolutionTransformerBaseTests, NextImageReturnsFalseIfParametersContainZero)
{
    mockTransformer.nextImageReturnValue = true;
    TestClass sut{mockTransformer};
    sut.matrixWidthValue = 0U;
    EXPECT_FALSE(sut.nextImage());
    sut.matrixWidthValue  = 3U;
    sut.matrixHeightValue = 0U;
    EXPECT_FALSE(sut.nextImage());
    sut.matrixHeightValue = 3U;
    sut.matrixShiftXValue = 0U;
    EXPECT_FALSE(sut.nextImage());
    sut.matrixShiftXValue = 1U;
    sut.matrixShiftYValue = 0U;
    EXPECT_FALSE(sut.nextImage());
    sut.matrixShiftYValue = 1U;
    EXPECT_TRUE(sut.nextImage());
    EXPECT_EQ(mockTransformer.countCalls(CallType::NextImage), 5U);
}

TEST_F(ConvolutionTransformerBaseTests, ResetReturningFalseDoesNotCauseCrashesWhenCallingTransformOrSkip)
{
    mockTransformer.resetReturnValue = true;
    TestClass sut{mockTransformer};
    sut.matrixShiftXValue = 0U;
    EXPECT_FALSE(sut.reset());

    BGRAPixel pixel{};
    EXPECT_EQ(sut.dimensions(), Rectangle{});
    EXPECT_FALSE(sut.transform(pixel));
    EXPECT_FALSE(sut.skip());
}

TEST_F(ConvolutionTransformerBaseTests, DimensionsCalculatedCorrectly)
{
    TestClass sut{mockTransformer};
    Scenario  scenario{};

    mockTransformer.resetReturnValue = true;
    do
    {
        mockTransformer.dimensionsReturnValue = Rectangle{scenario.imageX, scenario.imageY};

        sut.matrixWidthValue  = scenario.matrixX;
        sut.matrixHeightValue = scenario.matrixY;
        sut.matrixShiftXValue = scenario.shiftX;
        sut.matrixShiftYValue = scenario.shiftY;

        // transformer should return true even if area of dimensions is zero as this is handled by the image
        EXPECT_TRUE(sut.reset());

        auto const expected = expectedDimensions(scenario);
        ASSERT_EQ(sut.dimensions().width, expected.width);
        ASSERT_EQ(sut.dimensions().height, expected.height);
    } while (nextScenario(scenario));
}

TEST_F(ConvolutionTransformerBaseTests, DoesNotCallTransformOnBaseAsLongAsOnlySkipIsCalled)
{
    TestClass sut{mockTransformer};
    mockTransformer.resetReturnValue      = true;
    mockTransformer.dimensionsReturnValue = Rectangle{16U, 16U};

    EXPECT_TRUE(sut.reset());
    auto const callCount = sut.dimensions().area();
    for (auto i = 0U; i < callCount; ++i)
    {
        ASSERT_TRUE(sut.skip());
    }
    EXPECT_FALSE(sut.skip());

    EXPECT_EQ(mockTransformer.countCalls(CallType::Transform), 0U);
    EXPECT_EQ(mockTransformer.countCalls(CallType::Skip), 0U);
}

// TODO transform to TEST_P in order to test different shift values 1, 2, 3
TEST_F(ConvolutionTransformerBaseTests, ValuesAreHandedToConvoluteAsExpected)
{
    static constexpr std::uint32_t MatrixDim = 3U;

    using TestMatrix = std::array<std::array<BGRAPixel, MatrixDim>, MatrixDim>;

    class ValueTestClass : public ConvolutionTransformerBase<BGRAPixel>
    {
    public:
        ValueTestClass(IImageTransformer<BGRAPixel> &base) noexcept : ConvolutionTransformerBase<BGRAPixel>{base} {}

        void getParameters(std::uint32_t &matrixWidth,
                           std::uint32_t &matrixHeight,
                           std::uint32_t &matrixShiftX,
                           std::uint32_t &matrixShiftY) noexcept override
        {
            matrixWidth  = MatrixDim;
            matrixHeight = MatrixDim;
            matrixShiftX = 2U;
            matrixShiftY = 2U;
        }

        BGRAPixel convolute(BGRAPixel const **const matrix) noexcept override
        {
            for (auto y = 0U; y < MatrixDim; ++y)
            {
                for (auto x = 0U; x < MatrixDim; ++x)
                {
                    matrixCopy[y][x] = matrix[y][x];
                }
            }
            return matrix[0U][0U];
        }

        TestMatrix matrixCopy{};
    };

    auto const     imageDim = 16U;
    ValueTestClass sut{mockTransformer};
    mockTransformer.resetReturnValue      = true;
    mockTransformer.dimensionsReturnValue = Rectangle{imageDim, imageDim};
    mockTransformer.transformReturnValue  = true;
    mockTransformer.skipReturnValue       = true;
    mockTransformer.countUpPixel          = true;

    auto const checkMatrix = [&](std::uint32_t const y, std::uint32_t const x) noexcept {
        BGRAPixel expectedColor{};
        for (auto matY = 0U; matY < MatrixDim; ++matY)
        {
            for (auto matX = 0U; matX < MatrixDim; ++matX)
            {
                auto const imageOffset  = (x * 2U) + (y * 2U * imageDim);
                auto const matrixOffset = matX + (matY * imageDim);
                expectedColor.blue      = imageOffset + matrixOffset;
                EXPECT_EQ(sut.matrixCopy[matY][matX], expectedColor)
                    << "X: " << x << " Y: " << y << " MatX: " << matX << " MatY: " << matY;
            }
        }
    };

    BGRAPixel pixel{};
    EXPECT_TRUE(sut.reset());
    auto const dimensions = sut.dimensions();
    for (auto y = 0U; y < dimensions.height; ++y)
    {
        for (auto x = 0U; x < dimensions.height; ++x)
        {
            EXPECT_TRUE(sut.transform(pixel));
            checkMatrix(y, x);
        }
    }
}

TEST_F(ConvolutionTransformerBaseTests, DISABLED_CallsToBaseTransformerAsExpectedOnNormalOperation)
{
    auto const dim = 16U;
    TestClass  sut{mockTransformer};
    mockTransformer.resetReturnValue      = true;
    mockTransformer.dimensionsReturnValue = Rectangle{dim, dim};
    mockTransformer.transformReturnValue  = true;
    mockTransformer.skipReturnValue       = true;
    mockTransformer.countUpPixel          = true;

    sut.matrixShiftXValue = 2U;
    sut.matrixShiftYValue = 2U;
    EXPECT_TRUE(sut.reset());

    auto      pixelsToTransform = (sut.dimensions().width * 2U) + 2U;
    BGRAPixel pixel{};
    for (auto i = 0U; i < pixelsToTransform; ++i)
    {
        EXPECT_TRUE(sut.transform(pixel));
    }

    // we load 2 complete lines for the matrix and 2 for the shift and on each line we need one skip() call
    auto const expectedSkipCallCount = sut.matrixHeightValue + sut.matrixShiftYValue - 1U;
    EXPECT_EQ(mockTransformer.countCalls(CallType::Skip), expectedSkipCallCount);

    auto const lineLength = sut.matrixWidthValue + ((sut.dimensions().width - 1U) * sut.matrixShiftXValue);
    auto const expectedTransformCallCount = (4U * lineLength) + sut.matrixWidthValue + sut.matrixShiftXValue;
    EXPECT_EQ(mockTransformer.countCalls(CallType::Transform), expectedTransformCallCount);
}

TEST_F(ConvolutionTransformerBaseTests, TransformationOfFirstPixelAfterMultipleSkippedPixels)
{
    auto const dim = 16U;
    TestClass  sut{mockTransformer};
    mockTransformer.resetReturnValue      = true;
    mockTransformer.dimensionsReturnValue = Rectangle{dim, dim};
    mockTransformer.transformReturnValue  = true;
    mockTransformer.skipReturnValue       = true;
    mockTransformer.countUpPixel          = true;

    sut.matrixShiftXValue = 2U;
    sut.matrixShiftYValue = 2U;
    EXPECT_TRUE(sut.reset());

    // skip one line and a couple of pixels to test skipping of pixels on the wrapped transformer
    auto pixelsToSkip = sut.dimensions().width + 2U;
    for (auto i = 0U; i < pixelsToSkip; ++i)
    {
        EXPECT_TRUE(sut.skip());
    }
    BGRAPixel pixel{};
    for (auto i = 0U; i < pixelsToSkip; ++i)
    {
        EXPECT_TRUE(sut.transform(pixel));
        // check pixel has the right value
    }

    auto expectedSkipCallCount = 16U + 16U + 8U;
    EXPECT_EQ(mockTransformer.countCalls(CallType::Skip), expectedSkipCallCount);
    auto const lineLength = sut.matrixWidthValue + ((sut.dimensions().width - 1U) * sut.matrixShiftXValue);
    auto const expectedTransformCallCount = (4U * lineLength) + sut.matrixWidthValue + sut.matrixShiftXValue;
    EXPECT_EQ(mockTransformer.countCalls(CallType::Transform), expectedTransformCallCount);
}

} // namespace Terrahertz::UnitTests
