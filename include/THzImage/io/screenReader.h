#ifndef THZ_IMAGE_IO_SCREENREADER_H
#define THZ_IMAGE_IO_SCREENREADER_H

// functions used by this class are OS specific and we support only windows for now
#ifdef _WIN32

#include "THzCommon/math/rectangle.hpp"
#include "THzCommon/utility/staticPImpl.hpp"
#include "THzImage/common/iImageReader.h"
#include "THzImage/common/pixel.h"

#include <memory>

namespace Terrahertz::Screen {

class Reader : public IImageReader<BGRAPixel>
{
public:
    /// @brief Returns the dimensions of the screen,
    ///
    /// @return A rectangle containing the screen dimensions.
    static Rectangle getScreenDimensions() noexcept;

    /// @brief Initializes a new Screen::Reader.
    Reader() noexcept;

    /// @brief Initializes a new Screen::Reader using the given area for screenshots.
    ///
    /// @param area The area to make screenshots in, will be trimmed if too big.
    Reader(Rectangle const &area) noexcept;

    /// @brief Prevent copy construction by explicitly deleting the constructor.
    Reader(Reader const &other) noexcept = delete;

    /// @brief Prevent move construction by explicitly deleting the constructor.
    Reader(Reader &&other) noexcept = delete;

    /// @brief Prevent copy assignment by explicitly deleting the operator.
    Reader &operator=(Reader const &other) noexcept = delete;

    /// @brief Prevent move assignment by explicitly deleting the operator.
    Reader &operator=(Reader &&other) noexcept = delete;

    /// @brief Deinitializes the Reader.
    ~Reader() noexcept;

    /// @brief Sets the area on the screen that screenshots should be made from.
    ///
    /// @param area The area to use.
    /// @return True if the area fits inside the screen dimensions.
    /// @remarks The area can be retrieved via the dimensions of the reader.
    [[nodiscard]] bool setArea(Rectangle const &area) noexcept;

    /// @copydoc IImageReader::multipleImages
    [[nodiscard]] bool multipleImages() const noexcept override;

    /// @copydoc IImageReader::init
    [[nodiscard]] bool init() noexcept override;

    /// @copydoc IImageReader::dimensions
    [[nodiscard]] Rectangle dimensions() const noexcept override;

    /// @copydoc IImageReader::read
    [[nodiscard]] bool read(gsl::span<BGRAPixel> buffer) noexcept override;

    /// @copydoc IImageReader::deinit
    void deinit() noexcept override;

private:
    /// @brief Forward declaration of the implementation.
    struct Impl;

    /// @brief Pointer to the implementation.
    StaticPImpl<Impl, 40U> _impl{};
};

} // namespace Terrahertz::Screen

#endif // !_WIN32
#endif // !THZ_IMAGE_IO_SCREENREADER_H
