#ifndef THZ_IMAGE_COMMON_PIXEL_HPP
#define THZ_IMAGE_COMMON_PIXEL_HPP

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Terrahertz {

/// @brief Struct for a blue green read alpha pixel using 8 bits per channel.
struct BGRAPixel
{
    /// @brief Blue value of the pixel.
    std::uint8_t blue{};

    /// @brief Green value of the pixel.
    std::uint8_t green{};

    /// @brief Red value of the pixel.
    std::uint8_t red{};

    /// @brief Alpha value of the pixel.
    std::uint8_t alpha{0xFFU};

    /// @brief Initializes a new pixel using the BGRA color space.
    ///
    /// @param b Blue.
    /// @param g Green.
    /// @param r Red.
    /// @param a Alpha.
    constexpr BGRAPixel(std::uint8_t b, std::uint8_t g, std::uint8_t r, std::uint8_t a = 0xFFU) noexcept
        : blue{b}, green{g}, red{r}, alpha{a}
    {}

    /// @brief Default initializes a new BGRAPixel.
    constexpr BGRAPixel() noexcept = default;

    /// @brief Explicitly default the constructor so all special methods are defined.
    constexpr BGRAPixel(BGRAPixel const &) noexcept = default;

    /// @brief Explicitly default the constructor so all special methods are defined.
    constexpr BGRAPixel(BGRAPixel &&) noexcept = default;

    /// @brief Explicitly default the operator so all special methods are defined.
    BGRAPixel &operator=(BGRAPixel const &) noexcept = default;

    /// @brief Explicitly default the operator so all special methods are defined.
    BGRAPixel &operator=(BGRAPixel &&) noexcept = default;

    /// @brief Explicitly default the destructor so all special methods are defined.
    ~BGRAPixel() noexcept = default;

    /// @brief Checks if another pixel equals this one.
    ///
    /// @param other The other pixel.
    /// @returns True if both pixels are equal in every color and alpha, false otherwise.
    bool operator==(BGRAPixel const &other) const noexcept;

    /// @brief Checks if another pixel not equals this one.
    ///
    /// @param other The other pixel.
    /// @returns True if both pixels are different in at least one color or alpha, false otherwise.
    bool operator!=(BGRAPixel const &other) const noexcept;

    /// @brief Calculates the squared distance between the this pixel and the other one in the color cube.
    ///
    /// @param other The other pixel.
    /// @returns The squared distance between the pixels.
    size_t distanceSquared(BGRAPixel const &other) const noexcept;

    /// @brief The addition operator.
    ///
    /// @param other The other pixel to add.
    /// @return The resulting pixel.
    /// @remarks Alpha channel will be taken from this pixel.
    BGRAPixel operator+(BGRAPixel const &other) const noexcept;

    /// @brief The subtraction operator, calculating the diff between two pixels.
    ///
    /// @param other The other pixel to subtract.
    /// @return The resulting pixel.
    /// @remarks Alpha channel will be taken from this pixel.
    BGRAPixel operator-(BGRAPixel const &other) const noexcept;

    /// @brief Subtracts the smaller from the bigger number channel by channel and returns the result.
    ///
    /// @param other The other pixel to calculate the difference.
    /// @return The result of the calculation.
    BGRAPixel diffAbs(BGRAPixel const &other) const noexcept;
};

/// @brief Struct for a blue green read alpha pixel using a custom data type for the channels.
///
/// This struct offers operators for doing math with the regular BGRAPixel,
/// to for instance calculate the average color of a set of pixels.
/// @tparam T The data type for each channel.
template <typename T>
struct TemplatedBGRAPixel
{
    static_assert(!std::is_same_v<std::uint8_t, T>, "Use standard BGRAPixel instead");

    static_assert(std::is_arithmetic_v<T>, "Only available for arithmetic types");

    /// @brief Blue value of the pixel.
    T blue{};

    /// @brief Green value of the pixel.
    T green{};

    /// @brief Red value of the pixel.
    T red{};

    /// @brief Alpha value of the pixel.
    T alpha{0xFFU};

    /// @brief Initializes a new pixel using the BGRA color space.
    ///
    /// @param b Blue.
    /// @param g Green.
    /// @param r Red.
    /// @param a Alpha.
    TemplatedBGRAPixel(T b, T g, T r, T a = 0xFFU) noexcept : blue{b}, green{g}, red{r}, alpha{a} {}

    /// @brief Default initializes a new BGRAPixel.
    TemplatedBGRAPixel() noexcept = default;

    /// @brief Explicitly default the constructor so all special methods are defined.
    TemplatedBGRAPixel(TemplatedBGRAPixel const &) noexcept = default;

    /// @brief Copy-Constructor to convert from a BGRAPixel.
    ///
    /// @param other The pixel to convert.
    TemplatedBGRAPixel(BGRAPixel const &other) noexcept
        : blue{static_cast<T>(other.blue)},
          green{static_cast<T>(other.green)},
          red{static_cast<T>(other.red)},
          alpha{static_cast<T>(other.alpha)}
    {}

    /// @brief Explicitly default the constructor so all special methods are defined.
    TemplatedBGRAPixel(TemplatedBGRAPixel &&) noexcept = default;

    /// @brief Explicitly default the operator so all special methods are defined.
    TemplatedBGRAPixel &operator=(TemplatedBGRAPixel const &) noexcept = default;

    /// @brief Assignment operator to convert from a BGRAPixel.
    ///
    /// @param other The pixel to convert.
    TemplatedBGRAPixel &operator=(BGRAPixel const &other) noexcept
    {
        blue  = other.blue;
        green = other.green;
        red   = other.red;
        alpha = other.alpha;
        return *this;
    }

    /// @brief Explicitly default the operator so all special methods are defined.
    TemplatedBGRAPixel &operator=(TemplatedBGRAPixel &&) noexcept = default;

    /// @brief Explicitly default the destructor so all special methods are defined.
    ~TemplatedBGRAPixel() noexcept = default;

    /// @brief Cast operator to convert this instance into a regular BGRAPixel.
    ///
    /// @return A BGRAPixel, the Ting point value will be clamped to the std::uint8_t range.
    operator BGRAPixel() const noexcept
    {
        auto const convert = [](T value) noexcept -> std::uint8_t {
            return static_cast<std::uint8_t>(std::clamp(value + 0.01F, 0.0F, 255.0F));
        };
        return BGRAPixel{convert(blue), convert(green), convert(red), convert(alpha)};
    }

    /// @brief Adds a BGRAPixel to this one.
    ///
    /// @param addend The pixel to add.
    /// @return This pixel.
    TemplatedBGRAPixel &operator+=(BGRAPixel const &addend) noexcept
    {
        blue += addend.blue;
        green += addend.green;
        red += addend.red;
        alpha += addend.alpha;
        return *this;
    }

    /// @brief Adds a TemplatedBGRAPixel using the same T to this one.
    ///
    /// @param addend The pixel to add.
    /// @return This pixel.
    TemplatedBGRAPixel &operator+=(TemplatedBGRAPixel const &addend) noexcept
    {
        blue += addend.blue;
        green += addend.green;
        red += addend.red;
        alpha += addend.alpha;
        return *this;
    }

    /// @brief Subtracts a BGRAPixel from this one.
    ///
    /// @param subtrahend The pixel to subtract.
    /// @return This pixel.
    TemplatedBGRAPixel &operator-=(BGRAPixel const &subtrahend) noexcept
    {
        blue -= subtrahend.blue;
        green -= subtrahend.green;
        red -= subtrahend.red;
        alpha -= subtrahend.alpha;
        return *this;
    }

    /// @brief Subtracts a TemplatedBGRAPixel using the same T from this one.
    ///
    /// @param subtrahend The pixel to subtract.
    /// @return This pixel.
    TemplatedBGRAPixel &operator-=(TemplatedBGRAPixel const &subtrahend) noexcept
    {
        blue -= subtrahend.blue;
        green -= subtrahend.green;
        red -= subtrahend.red;
        alpha -= subtrahend.alpha;
        return *this;
    }

    /// @brief Multiplies this pixel by the given factor.
    ///
    /// @param factor The factor by which to multiply with.
    /// @return This pixel.
    TemplatedBGRAPixel &operator*=(T const factor) noexcept
    {
        blue *= factor;
        green *= factor;
        red *= factor;
        alpha *= factor;
        return *this;
    }

    /// @brief Divides this pixel by the given divisor.
    ///
    /// @param divisor The divisor by which to divide the pixel.
    /// @return This pixel.
    TemplatedBGRAPixel &operator/=(T const divisor) noexcept
    {
        blue /= divisor;
        green /= divisor;
        red /= divisor;
        alpha /= divisor;
        return *this;
    }
};

/// @brief Struct for HSVA pixel.
struct HSVAPixel
{
    /// @brief Hue of the pixel [rad] 0 - 2Pi.
    float hue{};

    /// @brief Saturation of the pixel.
    std::uint8_t saturation{};

    /// @brief Value of the pixel.
    std::uint8_t value{};

    /// @brief Alpha of the pixel.
    std::uint8_t alpha{0xFFU};

    /// @brief Initializes a new pixel using the HSVA color space.
    ///
    /// @param h Hue [rad].
    /// @param s Saturation.
    /// @param v Value.
    /// @param a Alpha.
    HSVAPixel(float h, std::uint8_t s, std::uint8_t v, std::uint8_t a = 0xFFU) noexcept
        : hue{h}, saturation{s}, value{v}, alpha{a}
    {}

    /// @brief Default initializes a new HSVAPixel.
    HSVAPixel() noexcept = default;

    /// @brief Explicitly default the constructor so all special methods are defined.
    HSVAPixel(HSVAPixel const &) noexcept = default;

    /// @brief Copy-Constructor to convert from a BGRAPixel.
    ///
    /// @param other The pixel to convert.
    HSVAPixel(BGRAPixel const &other) noexcept;

    /// @brief Explicitly default the constructor so all special methods are defined.
    HSVAPixel(HSVAPixel &&) noexcept = default;

    /// @brief Explicitly default the operator so all special methods are defined.
    HSVAPixel &operator=(HSVAPixel const &) noexcept = default;

    /// @brief Assignment operator to convert from a BGRAPixel.
    ///
    /// @param other The pixel to convert.
    HSVAPixel &operator=(BGRAPixel const &other) noexcept;

    /// @brief Explicitly default the operator so all special methods are defined.
    HSVAPixel &operator=(HSVAPixel &&) noexcept = default;

    /// @brief Explicitly default the destructor so all special methods are defined.
    ~HSVAPixel() noexcept = default;

    /// @brief Cast operator to convert this instance into a BGRAPixel.
    operator BGRAPixel() const noexcept;

    /// @brief Checks if another pixel equals this one.
    ///
    /// @param other The other pixel.
    /// @returns True if both pixels are equal in every channel including alpha, false otherwise.
    bool operator==(HSVAPixel const &other) const noexcept;

    /// @brief Checks if another pixel not equals this one.
    ///
    /// @param other The other pixel.
    /// @returns True if both pixels are different in at least one channel including alpha, false otherwise.
    bool operator!=(HSVAPixel const &other) const noexcept;
};

/// @brief Struct for a size reduced HSV pixel.
struct MiniHSVPixel
{
    /// @brief The content of this pixel.
    ///
    /// 7 6 5 4 3 2 1 0
    /// H H H S S V V V
    ///
    /// Hue 8 values [Pi]
    /// 0: 0.00 - 0.25
    /// 1: 0.25 - 0.50
    /// 2: 0.50 - 0.75
    /// 3: 0.75 - 1.00
    /// 4: 1.00 - 1.25
    /// 5: 1.25 - 1.50
    /// 6: 1.50 - 1.75
    /// 7: 1.75 - 2.00
    ///
    /// Saturation 4 values
    /// 0:   0 -  63
    /// 1:  64 - 127
    /// 2: 128 - 191
    /// 3: 192 - 255
    ///
    /// Value 8 Values
    /// 0:   0 -  31
    /// 1:  32 -  63
    /// 2:  64 -  95
    /// 3:  96 - 127
    /// 4: 128 - 159
    /// 5: 160 - 191
    /// 6: 192 - 223
    /// 7: 224 - 255
    std::uint8_t content{};

    /// @brief Default initializes a new MiniHSVPixel.
    MiniHSVPixel() noexcept = default;

    /// @brief Explicitly default the constructor so all special methods are defined.
    MiniHSVPixel(MiniHSVPixel const &) noexcept = default;

    /// @brief Copy-Constructor to convert from a BGRAPixel.
    ///
    /// @param other The pixel to convert.
    MiniHSVPixel(BGRAPixel const &other) noexcept;

    /// @brief Copy-Constructor to convert from a HSVAPixel.
    ///
    /// @param other The pixel to convert.
    MiniHSVPixel(HSVAPixel const &other) noexcept;

    /// @brief Explicitly default the constructor so all special methods are defined.
    MiniHSVPixel(MiniHSVPixel &&) noexcept = default;

    /// @brief Explicitly default the operator so all special methods are defined.
    MiniHSVPixel &operator=(MiniHSVPixel const &) noexcept = default;

    /// @brief Assignment operator to convert from a BGRAPixel.
    ///
    /// @param other The pixel to convert.
    MiniHSVPixel &operator=(BGRAPixel const &other) noexcept;

    /// @brief Assignment operator to convert from HSVAPixel.
    ///
    /// @param other The pixel to convert.
    MiniHSVPixel &operator=(HSVAPixel const &other) noexcept;

    /// @brief Explicitly default the operator so all special methods are defined.
    MiniHSVPixel &operator=(MiniHSVPixel &&) noexcept = default;

    /// @brief Explicitly default the destructor so all special methods are defined.
    ~MiniHSVPixel() noexcept = default;

    /// @brief Cast operator to convert this instance into a BGRAPixel.
    operator BGRAPixel() const noexcept;

    /// @brief Cast operator to convert this instance into a HSVAPixel.
    operator HSVAPixel() const noexcept;
};

/// @brief Performes a linear interpolation between the given color values.
///
/// @param a The first color.
/// @param b The second color.
/// @param t The interpolation value.
/// @returns The interpolated color value.
BGRAPixel lerp(BGRAPixel const &a, BGRAPixel const &b, float const t) noexcept;

/// @brief Performes a linear interpolation between the given color values.
///
/// @param a The first color.
/// @param b The second color.
/// @param t The interpolation value.
/// @returns The interpolated color value.
HSVAPixel lerp(HSVAPixel const &a, HSVAPixel const &b, float const t) noexcept;

/// @brief Shortcut to a templated BGRAPixel class using float.
using BGRAPixelFloat = TemplatedBGRAPixel<float>;

/// @brief Shortcut to a templated BGRAPixel class using std::uint32_t.
using BGRAPixel32 = TemplatedBGRAPixel<std::uint32_t>;

/// @brief Checks if the given type is a pixel type.
///
/// @tparam TPixelType The pixel type to check.
template <typename TPixelType>
struct is_pixel_type : std::false_type
{};

/// @brief Check for the BGRAPixel type.
template <>
struct is_pixel_type<BGRAPixel> : std::true_type
{};

/// @brief Check for the BGRAPixelFloat type.
template <>
struct is_pixel_type<BGRAPixelFloat> : std::true_type
{};

/// @brief Check for the BGRAPixel32 type.
template <>
struct is_pixel_type<BGRAPixel32> : std::true_type
{};

/// @brief Check for the HSVAPixel type.
template <>
struct is_pixel_type<HSVAPixel> : std::true_type
{};

/// @brief Concept for a pixel type.
template <typename T>
concept Pixel = std::default_initializable<T> && std::copyable<T> && std::movable<T> &&
                std::convertible_to<T, BGRAPixel> && std::convertible_to<BGRAPixel, T>;

} // namespace Terrahertz

#endif // !THZ_IMAGE_COMMON_PIXEL_HPP
