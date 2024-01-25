#ifndef THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMERBASE_HPP
#define THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMERBASE_HPP

#include "THzCommon/logging/logging.hpp"
#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/iImageTransformer.hpp"

#include <vector>

namespace Terrahertz {

/// @brief Name provider for the THzImage.Transformation.Convolution project.
struct ConvolutionTransformerProject
{
    static constexpr char const *name() noexcept { return "THzImage.Transformation.Convolution"; }
};

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
    bool transform(TPixelType &pixel) noexcept override
    {
        // prepare buffer content
        for (; _buffer.count > 0U; --_buffer.count)
        {
            // skip transformations of pixels not needed for future matrizes
            if (!((_buffer.count > _buffer.size) ? _wrapped->skip() : _wrapped->transform(*_buffer.curPtr)))
            {
                return false;
            }
            ++_buffer.curPtr;
            // execute skips at the end of the line
            if (_buffer.curPtr == _buffer.linePtr)
            {
                for (auto i = 0U; i < _buffer.skips; ++i)
                {
                    if (!_wrapped->skip())
                    {
                        return false;
                    }
                    _buffer.linePtr += _buffer.lineLength;
                    if (_buffer.linePtr > _buffer.endPtr)
                    {
                        // go back to the first line if buffer end is reached
                        _buffer.linePtr -= _buffer.memory.size();
                    }
                }
            }
            if (_buffer.curPtr == _buffer.endPtr)
            {
                _buffer.curPtr -= _buffer.memory.size();
            }
        }
        pixel = convolute(_matrix.rows.data());
        return skip();
    }

    /// @copydoc IImageTransformer::skip
    bool skip() noexcept override
    {
        _buffer.count += _matrix.shift;
        for (auto &row : _matrix.rows)
        {
            row += _matrix.shift;
            if (row == _matrix.lineEnd)
            {
                for (auto &row2 : _matrix.rows)
                {
                    row2 += _matrix.skip;
                }
                _buffer.count += _matrix.lineShift;
            }
            if (row >= _buffer.endPtr)
            {
                row -= _buffer.memory.size();
            }
        }
        if (_pixelsLeft)
        {
            --_pixelsLeft;
            return true;
        }
        return false;
    }

    /// @copydoc IImageTransformer::reset
    bool reset() noexcept override
    {
        if (!_wrapped->reset())
        {
            return false;
        }
        return setup();
    }

    /// @copydoc IImageTransformer::nextImage
    bool nextImage() noexcept override
    {
        if (!_wrapped->nextImage())
        {
            return false;
        }
        return setup();
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
    ///
    /// @return True if all values returned by getParameters or non-zero, false otherwise.
    bool setup() noexcept
    {
        std::uint32_t matrixWidth;
        std::uint32_t matrixHeight;
        std::uint32_t matrixShiftX;
        std::uint32_t matrixShiftY;
        getParameters(matrixWidth, matrixHeight, matrixShiftX, matrixShiftY);
        if (matrixWidth == 0U)
        {
            logMessage<LogLevel::Error, ConvolutionTransformerProject>("matrixWidth is 0");
            handleParameterError();
            return false;
        }
        if (matrixHeight == 0U)
        {
            logMessage<LogLevel::Error, ConvolutionTransformerProject>("matrixHeight is 0");
            handleParameterError();
            return false;
        }
        if (matrixShiftX == 0U)
        {
            logMessage<LogLevel::Error, ConvolutionTransformerProject>("matrixShiftX is 0");
            handleParameterError();
            return false;
        }
        if (matrixShiftY == 0U)
        {
            logMessage<LogLevel::Error, ConvolutionTransformerProject>("matrixShiftY is 0");
            handleParameterError();
            return false;
        }

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

        _resultDimensions = Rectangle{calcDim(baseDimensions.width, matrixWidth, matrixShiftX),
                                      calcDim(baseDimensions.height, matrixHeight, matrixShiftY)};
        _pixelsLeft       = _resultDimensions.area();
        setupInternalStructures(baseDimensions.width, matrixWidth, matrixHeight, matrixShiftX, matrixShiftY);
        return true;
    }

    /// @brief
    void handleParameterError() noexcept
    {
        _resultDimensions = {};
        _pixelsLeft       = 0U;
        setupInternalStructures(10U, 10U, 10U, 1U, 1U);
        _buffer.count = 0U;
    }

    void setupInternalStructures(std::uint32_t const baseWidth,
                                 std::uint32_t const matrixWidth,
                                 std::uint32_t const matrixHeight,
                                 std::uint32_t const matrixShiftX,
                                 std::uint32_t const matrixShiftY) noexcept
    {
        _buffer.lineLength = matrixWidth + ((_resultDimensions.width - 1U) * matrixShiftX);
        _buffer.memory.resize(_buffer.lineLength * matrixHeight);
        _buffer.curPtr  = _buffer.memory.data();
        _buffer.linePtr = _buffer.curPtr + _buffer.lineLength;
        _buffer.endPtr  = _buffer.curPtr + _buffer.memory.size();
        _buffer.size    = (_buffer.lineLength * (matrixHeight - 1U)) + matrixWidth;
        _buffer.count   = _buffer.size;
        _buffer.skips   = baseWidth - _buffer.lineLength;

        _matrix.rows.clear();
        for (auto line = 0U; line < matrixHeight; ++line)
        {
            _matrix.rows.push_back(_buffer.memory.data() + (_buffer.lineLength * line));
        }
        _matrix.shift     = matrixShiftX;
        _matrix.lineEnd   = _matrix.rows[0U] + (matrixShiftX * _resultDimensions.width);
        _matrix.lineShift = matrixShiftY * _buffer.lineLength;
        _matrix.skip      = matrixWidth + _matrix.lineShift;
    }

    /// @brief The base transformer.
    IImageTransformer<TPixelType> *_wrapped{};

    /// @brief The dimensions of the resulting image.
    Rectangle _resultDimensions{};

    std::uint32_t _pixelsLeft{};

    struct Buffer
    {
        std::vector<TPixelType> memory{};

        TPixelType *curPtr{};

        TPixelType *linePtr{};

        TPixelType *endPtr{};

        std::uint32_t count{};

        std::uint32_t lineLength{};

        std::uint32_t size{};

        std::uint32_t skips{};

    } _buffer;

    struct Matrix
    {
        std::vector<TPixelType const *> rows{};

        std::uint32_t shift{};

        std::uint32_t skip{};

        std::uint32_t lineShift{};

        TPixelType const *lineEnd{};
    } _matrix;
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMERBASE_HPP
