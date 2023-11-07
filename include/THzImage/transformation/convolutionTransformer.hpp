#ifndef THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMER_HPP
#define THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMER_HPP

#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/iImageTransformer.hpp"

#include <concepts>
#include <cstdint>
#include <stdexcept>

namespace Terrahertz {

/// @brief Checks and stores the paramters of convolution transformation.
///
/// @tparam TPixelType The pixel type of the transformation.
template <typename TPixelType>
class ConvolutionParameters
{
public:
    /// @brief Initializes a new set of parameters.
    ///
    /// @param pSizeX The size of the matrix on the x-axis.
    /// @param pSizeY The size of the matrix on the y-axis.
    /// @param pShiftX The amount of pixels on the x-axis the matrix is shifted each step.
    /// @param pShiftY The amount of pixels on the y-axis the matrix is shifted each step.
    /// @param pBorder True if the transformer shall add a border, false otherwise.
    /// @param pBorderFill The color the border should be filled with, ignored if border is false.
    /// @remark The added border will:
    ///           - help to preserve the original size if shift is 1
    ///           - round up the number of steps of shift is greater than 1
    /// @throws invalid_argument In case sizeX, sizeY, shiftX or shiftY is zero.
    ConvolutionParameters(std::uint16_t const pSizeX,
                          std::uint16_t const pSizeY,
                          std::uint16_t const pShiftX,
                          std::uint16_t const pShiftY,
                          bool const          pBorder,
                          TPixelType const    pBorderFill) noexcept(false)
        : _sizeX{pSizeX}, _sizeY{pSizeY}, _shiftX{pShiftX}, _shiftY{pShiftY}, _border{pBorder}, _borderFill{pBorderFill}
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

    /// @brief Returns true if the transformer shall add a border, false otherwise.
    ///
    /// @return True if the transformer shall add a border, false otherwise.
    inline bool border() const noexcept { return _border; }

    /// @brief Returns the color the border should be filled with, ignored if border is false.
    ///
    /// @return The color the border should be filled with, ignored if border is false.
    inline TPixelType borderFill() const noexcept { return _borderFill; }

private:
    /// @brief The size of the matrix on the x-axis.
    std::uint16_t _sizeX{};

    /// @brief The size of the matrix on the y-axis.
    std::uint16_t _sizeY{};

    /// @brief The amount of pixels on the x-axis the matrix is shifted each step.
    std::uint16_t _shiftX{};

    /// @brief The amount of pixels on the y-axis the matrix is shifted each step.
    std::uint16_t _shiftY{};

    /// @brief True if the transformer shall add a border, false otherwise.
    bool _border{};

    /// @brief The color the border should be filled with, ignored if border is false.
    TPixelType _borderFill{};
};

// clang-format off

template <typename TType, typename TPixelType>
concept ConvolutionTransformation = requires(TType t, TPixelType **matrix)
{
	{t.parameters()} -> std::same_as<ConvolutionParameters<TPixelType>>;

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
{};

} // namespace Terrahertz

#endif // !THZ_IMAGE_HANDLING_CONVOLUTIONTRANSFORMER_HPP
