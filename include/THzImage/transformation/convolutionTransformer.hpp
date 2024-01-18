#ifndef THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMER_HPP
#define THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMER_HPP

#include "THzCommon/logging/logging.hpp"
#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/iImageTransformer.hpp"

#include <concepts>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace Terrahertz {

// Move helper classes to internal namespace to enable testing
namespace Internal {

/// @brief Stores the lines needed for running the transformation.
///
/// @tparam TPixelType The type of pixel used by the transformer.
template <typename TPixelType>
class LineBuffer
{
public:
    /// @brief Uses the given values to configure the buffer.
    ///
    /// @param size The new size of the buffer.
    /// @param lineLength The length of the line in this buffer [pxl].
    /// @param pixelsToSkip The pixels at the end of each line in the base.
    void setup(std::uint32_t const size, std::uint32_t const lineLength, std::uint32_t const pixelsToSkip) noexcept
    {
        _memory.resize(size);
        _lineLength   = lineLength;
        _pixelsToSkip = pixelsToSkip;
        _curPtr       = _memory.data();
        _endPtr       = _curPtr + size;
    }

    /// @brief Reads the next line from the given base transformer.
    ///
    /// @param base The base transformer to read from.
    /// @return True if operation was successful, false otherwise.
    bool readNextLine(IImageTransformer<TPixelType> &base) noexcept
    {
        for (auto i = 0U; i < _lineLength; ++i)
        {
            if (!base.transform(*_curPtr))
            {
                // reset the pointer so other operations do not cause access violations
                _curPtr = _memory.data();
                return false;
            }
            ++_curPtr;
        }
        // we can put this here because we reset _curPtr in case of an error
        if (_curPtr == _endPtr)
        {
            _curPtr = _memory.data();
        }
        for (auto i = 0U; i < _pixelsToSkip; ++i)
        {
            if (!base.skip())
            {
                // reset the pointer so other operations do not cause access violations
                _curPtr = _memory.data();
                return false;
            }
        }
        return true;
    }

    /// @brief Returns the pointer to the first pixel in the buffer.
    ///
    /// @return The pointer to the first pixel in the buffer.
    TPixelType const *data() const noexcept { return _memory.data(); }

private:
    /// @brief The memory to store the lines.
    std::vector<TPixelType> _memory{};

    /// @brief The pointer to the next slot to write.
    TPixelType *_curPtr{};

    /// @brief The pointer to the first address after the buffer.
    TPixelType *_endPtr{};

    /// @brief The length of the line in this buffer [pxl].
    std::uint32_t _lineLength{};

    /// @brief The pixels at the end of each line in the base.
    std::uint32_t _pixelsToSkip{};
};

/// @brief Used for stepping through the line buffer and creating the matrizes for the transformation.
///
/// @tparam TPixelType The type of pixel used by the transformer.
template <typename TPixelType>
class MatrixHelper
{
public:
    /// @brief Resets the helper and enters all information to convert the line buffer.
    ///
    /// @param buffer Pointer to the first pixel of the line buffer.
    /// @param lineLength The length of the lines.
    /// @param lineCount The amount of lines.
    /// @param matrixWidth The width of the matrix.
    /// @param matrixShift The amount of pixels the matrix is shifted each step.
    void setup(TPixelType const *const buffer,
               std::uint32_t const     lineLength,
               std::uint32_t const     lineCount,
               std::uint32_t const     matrixWidth,
               std::uint32_t const     matrixShift) noexcept
    {
        _lineLength  = lineLength;
        _matrixShift = matrixShift;
        _rows.clear();
        auto lineStart = buffer;
        for (auto i = 0U; i < lineCount; ++i)
        {
            _rows.push_back(lineStart);
            lineStart += lineLength;
        }

        auto const remain = _lineLength - matrixWidth;
        auto const steps  = 1U + (remain / _matrixShift);
        _end              = _rows[0U] + (steps * _matrixShift);
        _lineEndSkip      = matrixWidth - 1U;
        _bufferLength     = _lineLength * lineCount;
        _bufferEnd        = buffer + _bufferLength;
        _exhausted        = false;
    }

    /// @brief Returns the current matrix to feed into the transformation.
    ///
    /// @return The current matrix to feed into the transformation.
    TPixelType const **operator()() noexcept { return _rows.data(); }

    /// @brief Moves the matrix to the next location on the line buffer.
    ///
    /// @return True if another matrix can be read from the current location, false otherwise.
    bool next() noexcept
    {
        if (_exhausted)
        {
            return false;
        }
        for (auto &row : _rows)
        {
            row += _matrixShift;
            if (row == _end)
            {
                _exhausted = true;
            }
        }
        return !_exhausted;
    }

    /// @brief Once the end of the buffer has been reached, this method can be used to forward to the next line.
    ///
    /// @param additionalLines The number of additional lines to skip.
    void lineFeed(std::uint32_t const additionalLines) noexcept
    {
        for (auto &row : _rows)
        {
            row += _lineEndSkip;
            if (row == _bufferEnd)
            {
                row -= _bufferLength;
            }
        }
        for (auto i = 0U; i < additionalLines; ++i)
        {
            for (auto &row : _rows)
            {
                row += _lineLength;
                if (row == _bufferEnd)
                {
                    row -= _bufferLength;
                }
            }
        }
        _exhausted = false;
    }

private:
    /// @brief The vector for the row pointers.
    std::vector<TPixelType const *> _rows{};

    /// @brief The length of the lines.
    std::uint16_t _lineLength{};

    /// @brief The pixels to skip at the end of the line to reach the beginning of the next line.
    std::uint16_t _lineEndSkip{};

    /// @brief The amount of pixels the matrix is shifted each step.
    std::uint16_t _matrixShift{};

    /// @brief The length of the line buffer.
    std::uint32_t _bufferLength{};

    /// @brief Pointer to the end of the line.
    TPixelType const *_end{};

    /// @brief Pointer to the end of the buffer.
    TPixelType const *_bufferEnd{};

    /// @brief Flag signalling if the next call to operator() would cause an access violation.
    bool _exhausted{};
};

} // namespace Internal

/// @brief Name provider for the THzImage.IO.BMP.Reader class.
struct ConvolutionTransformerProject
{
    static constexpr char const *name() noexcept { return "THzImage.Transformation.Convolution"; }
};

/// @brief Checks and stores the paramters of convolution transformation.
class ConvolutionParameters
{
public:
    /// @brief Initializes a new set of parameters.
    ///
    /// @param pSizeX The size of the matrix on the x-axis.
    /// @param pSizeY The size of the matrix on the y-axis.
    /// @param pShiftX The amount of pixels on the x-axis the matrix is shifted each step.
    /// @param pShiftY The amount of pixels on the y-axis the matrix is shifted each step.
    /// @throws invalid_argument In case sizeX, sizeY, shiftX or shiftY is zero.
    ConvolutionParameters(std::uint16_t const pSizeX,
                          std::uint16_t const pSizeY,
                          std::uint16_t const pShiftX,
                          std::uint16_t const pShiftY) noexcept(false)
        : _sizeX{pSizeX}, _sizeY{pSizeY}, _shiftX{pShiftX}, _shiftY{pShiftY}
    {
        if (_sizeX == 0U)
        {
            throw std::invalid_argument("sizeX is zero");
        }
        if (_sizeY == 0U)
        {
            throw std::invalid_argument("sizeY is zero");
        }
        if (_shiftX == 0U)
        {
            throw std::invalid_argument("shiftX is zero");
        }
        if (_shiftY == 0U)
        {
            throw std::invalid_argument("shiftY is zero");
        }
    }

    /// @brief Returns the size of the matrix on the x-axis.
    ///
    /// @return The size of the matrix on the x-axis.
    inline std::uint16_t sizeX() const noexcept { return _sizeX; }

    /// @brief Returns the size of the matrix on the y-axis.
    ///
    /// @return The size of the matrix on the y-axis.
    inline std::uint16_t sizeY() const noexcept { return _sizeY; }

    /// @brief Returns the amount of pixels on the x-axis the matrix is shifted each step.
    ///
    /// @return The amount of pixels on the x-axis the matrix is shifted each step.
    inline std::uint16_t shiftX() const noexcept { return _shiftX; }

    /// @brief Returns the amount of pixels on the y-axis the matrix is shifted each step.
    ///
    /// @return The amount of pixels on the y-axis the matrix is shifted each step.
    inline std::uint16_t shiftY() const noexcept { return _shiftY; }

private:
    /// @brief The size of the matrix on the x-axis.
    std::uint16_t _sizeX{};

    /// @brief The size of the matrix on the y-axis.
    std::uint16_t _sizeY{};

    /// @brief The amount of pixels on the x-axis the matrix is shifted each step.
    std::uint16_t _shiftX{};

    /// @brief The amount of pixels on the y-axis the matrix is shifted each step.
    std::uint16_t _shiftY{};
};

// clang-format off

template <typename TType, typename TPixelType>
concept ConvolutionTransformation = requires(TType t, TPixelType const **matrix)
{
	{t.parameters()} -> std::same_as<ConvolutionParameters>;

    // [y][x]
	{t(matrix)} -> std::same_as<TPixelType>;
};

// clang-format on

/// @brief Class wrapping Pixel-Matrix-to-Pixel transformation algorithms
///        to make them implement the IImageTransformer interface.
///
/// @tparam TPixelType The type of pixel used by the transformer.
/// @tparam TTransformation The type of transformation of the class.
template <typename TPixelType, ConvolutionTransformation<TPixelType> TTransformation>
class ConvolutionTransformer : public IImageTransformer<TPixelType>
{
public:
    /// @brief Initializes a new ConvolutionTransformer using the given values.
    ///
    /// @param base The base transformer to wrap.
    /// @param transformation The instance encapsulating the transformation algorithm.
    ConvolutionTransformer(IImageTransformer<TPixelType> &base, TTransformation transformation) noexcept
        : _base{&base}, _transformation{transformation}, _parameters{transformation.parameters()}
    {
        setup();
    }

    /// @copydoc IImageTransformer::dimensions
    Rectangle dimensions() const noexcept override { return _resultDimensions; }

    /// @copydoc IImageTransformer::transform
    bool transform(TPixelType &pixel) noexcept override
    {
        pixel = _transformation(_matrixHelper());
        return skip();
    }

    /// @copydoc IImageTransformer::skip
    bool skip() noexcept override
    {
        if (_matrixHelper.next())
        {
            return true;
        }
        bool result{};

        auto const feed = _parameters.shiftY();
        for (auto i = 0U; i < feed; ++i)
        {
            if (!_lineBuffer.readNextLine(*_base))
            {
                return false;
            }
        }
        _matrixHelper.lineFeed(feed - 1U);
        return true;
    }

    /// @copydoc IImageTransformer::reset
    bool reset() noexcept override
    {
        if (_base->reset())
        {
            return setup();
        }
        return false;
    }

    /// @copydoc IImageTransformer::nextImage
    bool nextImage() noexcept override
    {
        if (_base->nextImage())
        {
            return setup();
        }
        return false;
    }

private:
    /// @brief Performs the setup of the transformer, after construction, reset or nextImage.
    ///
    /// @return True if setup was successful, false otherwise.
    bool setup() noexcept
    {
        auto const calcDim = [](std::uint32_t const image,
                                std::uint16_t const matrix,
                                std::uint16_t const shift) noexcept -> std::uint32_t {
            if (image < matrix)
            {
                return 0U;
            }
            return ((image - matrix) / shift) + 1U;
        };

        auto const &params         = _parameters;
        auto const  baseDimensions = _base->dimensions();
        _resultDimensions.width    = calcDim(baseDimensions.width, params.sizeX(), params.shiftX());
        _resultDimensions.height   = calcDim(baseDimensions.height, params.sizeY(), params.shiftY());
        if (_resultDimensions.area() == 0U)
        {
            return false;
        }

        auto const lineLength   = params.sizeX() + ((_resultDimensions.width - 1U) * params.shiftX());
        auto const pixelsToSkip = baseDimensions.width - lineLength;
        auto const bufferSize   = lineLength * params.sizeY();
        _lineBuffer.setup(bufferSize, lineLength, pixelsToSkip);
        _matrixHelper.setup(_lineBuffer.data(), lineLength, params.sizeY(), params.sizeX(), params.shiftX());
        for (auto i = 0U; i < params.sizeY(); ++i)
        {
            if (!_lineBuffer.readNextLine(*_base))
            {
                return false;
            }
        }
        return true;
    }

    /// @brief The base transformer to wrap.
    IImageTransformer<TPixelType> *_base{};

    /// @brief The transformation to wrap.
    TTransformation _transformation;

    /// @brief The parameters of the transformation.
    ConvolutionParameters _parameters;

    /// @brief The dimensions of the resulting image.
    Rectangle _resultDimensions{};

    Internal::LineBuffer<TPixelType> _lineBuffer;

    Internal::MatrixHelper<TPixelType> _matrixHelper;
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMER_HPP
