#ifndef THZ_IMAGE_COMMON_PIXEL_HPP
#define THZ_IMAGE_COMMON_PIXEL_HPP

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Terrahertz {

// Predeclare the other pixel type
struct HSVAPixel;

/// @brief Struct for a BGRA pixel.
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
    BGRAPixel(std::uint8_t b, std::uint8_t g, std::uint8_t r, std::uint8_t a = 0xFFU) noexcept
        : blue{b}, green{g}, red{r}, alpha{a}
    {}

    /// @brief Default initializes a new BGRAPixel.
    BGRAPixel() noexcept = default;

    /// @brief Explicitly default the constructor so all special methods are defined.
    BGRAPixel(BGRAPixel const &) noexcept = default;

    /// @brief Copy-Constructor to convert from a HSVAPixel.
    ///
    /// @param other The pixel to convert.
    BGRAPixel(HSVAPixel const &other) noexcept;

    /// @brief Explicitly default the constructor so all special methods are defined.
    BGRAPixel(BGRAPixel &&) noexcept = default;

    /// @brief Explicitly default the operator so all special methods are defined.
    BGRAPixel &operator=(BGRAPixel const &) noexcept = default;

    /// @brief Assignment operator to convert from a HSVAPixel.
    ///
    /// @param other The pixel to convert.
    BGRAPixel &operator=(HSVAPixel const &other) noexcept;

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

struct BGRAPixelFloat
{
    /// @brief Blue value of the pixel.
    float blue{};

    /// @brief Green value of the pixel.
    float green{};

    /// @brief Red value of the pixel.
    float red{};

    /// @brief Alpha value of the pixel.
    float alpha{0xFFU};

    /// @brief Initializes a new pixel using the BGRA color space.
    ///
    /// @param b Blue.
    /// @param g Green.
    /// @param r Red.
    /// @param a Alpha.
    BGRAPixelFloat(float b, float g, float r, float a = 0xFFU) noexcept : blue{b}, green{g}, red{r}, alpha{a} {}

    /// @brief Default initializes a new BGRAPixel.
    BGRAPixelFloat() noexcept = default;

    /// @brief Explicitly default the constructor so all special methods are defined.
    BGRAPixelFloat(BGRAPixelFloat const &) noexcept = default;

    /// @brief Copy-Constructor to convert from a BGRAPixel.
    ///
    /// @param other The pixel to convert.
    BGRAPixelFloat(BGRAPixel const &other) noexcept;

    /// @brief Explicitly default the constructor so all special methods are defined.
    BGRAPixelFloat(BGRAPixelFloat &&) noexcept = default;

    /// @brief Explicitly default the operator so all special methods are defined.
    BGRAPixelFloat &operator=(BGRAPixelFloat const &) noexcept = default;

    /// @brief Assignment operator to convert from a BGRAPixel.
    ///
    /// @param other The pixel to convert.
    BGRAPixelFloat &operator=(BGRAPixel const &other) noexcept;

    /// @brief Explicitly default the operator so all special methods are defined.
    BGRAPixelFloat &operator=(BGRAPixelFloat &&) noexcept = default;

    /// @brief Explicitly default the destructor so all special methods are defined.
    ~BGRAPixelFloat() noexcept = default;

    /// @brief Cast operator to convert this instance into a regular BGRAPixel.
    ///
    /// @return A BGRAPixel, the float value will be clamped to the std::uint8_t range.
    operator BGRAPixel() const noexcept;
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

/// @brief Check for the HSVAPixel type.
template <>
struct is_pixel_type<HSVAPixel> : std::true_type
{};

} // namespace Terrahertz

#endif // !THZ_IMAGE_COMMON_PIXEL_HPP
