#ifndef THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMERBASE_HPP
#define THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMERBASE_HPP

#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/iImageTransformer.hpp"

#include <span>
#include <vector>

namespace Terrahertz {

/// @brief Base class for convolution based transformations.
///
/// @tparam TPixelType The type of pixel used by the transformation.
template <typename TPixelType>
class ConvolutionTransformerBase : public IImageTransformer<TPixelType>
{
public:
    /// @brief Initializes a new ConvolutionTransformerBase wrapping the given transformer.
    ///
    /// @param wrapped The transformer to wrap.
    ConvolutionTransformerBase(IImageTransformer<TPixelType> &wrapped) noexcept : _wrapped{&wrapped} {}

    /// @copydoc IImageTransformer::dimensions
    Rectangle dimensions() const noexcept override { return _resultDimensions; }

    /// @copydoc IImageTransformer::transform
    bool transform(TPixelType &) noexcept override { return false; }

    /// @copydoc IImageTransformer::skip
    bool skip() noexcept override { return (_count--) != 0U; }

    /// @copydoc IImageTransformer::reset
    bool reset() noexcept override
    {
        if (!_wrapped->reset())
        {
            return false;
        }
        setup();
        return true;
    }

    /// @copydoc IImageTransformer::nextImage
    bool nextImage() noexcept override
    {
        if (!_wrapped->nextImage())
        {
            return false;
        }
        setup();
        return true;
    }

    /// @brief This method is used by the base to retrieve the convolution parameters from the extending class.
    ///
    /// @param matrixWidth The width of the convolution matrix.
    /// @param matrixHeight The height of the convolution matrix.
    /// @param matrixShiftX The pixels the matrix gets shifted each step on the x-axis.
    /// @param matrixShiftY The lines the matrix gets shifted on the y-axis each time it has reached the end of a line.
    virtual void getParameters(std::uint32_t &matrixWidth,
                               std::uint32_t &matrixHeight,
                               std::uint32_t &matrixShiftX,
                               std::uint32_t &matrixShiftY) noexcept = 0;

    /// @brief This method is the filter kernel for the convolution implementation.
    /// The base class will extract matrizes from the image going left to right,
    /// top to bottom using the parameters taken from getParameters(...).
    /// The task if this method is turning the given matrix into a single pixel that can then be handed to the next
    /// transformer in the processing chain.
    ///
    /// @param matrix The matrix to convolute.
    /// @return The pixel calculated from the content of the matrix.
    virtual TPixelType convolute(TPixelType const **const matrix) noexcept = 0;

private:
    /// @brief This method will be called on creation, reset and nextImage to calculate base values needed for executing
    /// the transformation.
    void setup() noexcept
    {
        auto const calcDim = [](std::uint32_t const image,
                                std::uint32_t const matrix,
                                std::uint32_t const shift) noexcept -> std::uint32_t {
            if (image < matrix)
            {
                return 0U;
            }
            return ((image - matrix) / shift) + 1U;
        };
        auto const baseDimensions = _wrapped->dimensions();

        std::uint32_t matrixWidth;
        std::uint32_t matrixHeight;
        std::uint32_t matrixShiftX;
        std::uint32_t matrixShiftY;
        getParameters(matrixWidth, matrixHeight, matrixShiftX, matrixShiftY);
        _resultDimensions = Rectangle{calcDim(baseDimensions.width, matrixWidth, matrixShiftX),
                                      calcDim(baseDimensions.height, matrixHeight, matrixShiftY)};

        _count = _resultDimensions.area();
    }

    /// @brief The base transformer.
    IImageTransformer<TPixelType> *_wrapped{};

    /// @brief The dimensions of the resulting image.
    Rectangle _resultDimensions{};

    std::uint32_t _count{};
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMERBASE_HPP
