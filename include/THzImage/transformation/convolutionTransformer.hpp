#ifndef THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMER_HPP
#define THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMER_HPP

#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/iImageTransformer.hpp"

#include <concepts>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace Terrahertz {

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
    bool transform(TPixelType &pixel) noexcept override { return false; }

    /// @copydoc IImageTransformer::skip
    bool skip() noexcept override { return false; }

    /// @copydoc IImageTransformer::reset
    bool reset() noexcept override
    {
        if (_base->reset())
        {
            setup();
            return true;
        }
        return false;
    }

    /// @copydoc IImageTransformer::nextImage
    bool nextImage() noexcept override
    {
        if (_base->nextImage())
        {
            setup();
            return true;
        }
        return false;
    }

private:
    /// @brief Performs the setup of the transformer, after construction, reset or nextImage.
    void setup() noexcept
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

        _baseDimensions          = _base->dimensions();
        _resultDimensions.width  = calcDim(_baseDimensions.width, _parameters.sizeX(), _parameters.shiftX());
        _resultDimensions.height = calcDim(_baseDimensions.height, _parameters.sizeY(), _parameters.shiftY());
    }

    /// @brief The base transformer to wrap.
    IImageTransformer<TPixelType> *_base{};

    /// @brief The transformation to wrap.
    TTransformation _transformation;

    /// @brief The parameters of the transformation.
    ConvolutionParameters _parameters;

    /// @brief The dimensions of the base image.
    Rectangle _baseDimensions{};

    /// @brief The dimensions of the resulting image.
    Rectangle _resultDimensions{};
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMER_HPP
