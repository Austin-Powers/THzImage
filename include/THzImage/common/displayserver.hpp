#ifndef THZ_IMAGE_COMMON_DISPLAYSERVER_HPP
#define THZ_IMAGE_COMMON_DISPLAYSERVER_HPP
#ifdef __linux__

// Switch used to decide witch display server is used by the system
// and should be used by the programms using THzImage

#if defined(USING_X11)
#define THZ_IMAGE_X11_SCREENREADER_USED
#include <X11/Xlib.h>
#elif defined(USING_WAYLAND)
#define THZ_IMAGE_WAYLAND_SCREENREADER_USED
#else
#define THZ_IMAGE_DUMMY_SCREENREADER_USED
#endif // USING_X11

namespace Terrahertz {

/// @brief A connection to the display server.
class DisplayServerConnection
{
public:
    /// @brief The type of the connection stored in instance of this class.
#if defined (THZ_IMAGE_X11_SCREENREADER_USED)
    using ConnectionType = Display*;
#elif defined(THZ_IMAGE_WAYLAND_SCREENREADER_USED)
    using ConnectionType = int*;
#elif defined(THZ_IMAGE_DUMMY_SCREENREADER_USED)
    using ConnectionType = int*;
#endif

    /// @brief Returns the global connection to the display server.
    static DisplayServerConnection &globalInstance() noexcept;

    DisplayServerConnection() noexcept;

    DisplayServerConnection(DisplayServerConnection const &) noexcept = delete;

    DisplayServerConnection(DisplayServerConnection && other) noexcept;

    DisplayServerConnection &operator=(DisplayServerConnection const &) noexcept = delete;

    DisplayServerConnection &operator=(DisplayServerConnection &&other) noexcept;

    ~DisplayServerConnection() noexcept;

    ConnectionType operator()() noexcept;

private:
    /// @brief
    ConnectionType _connection{};
};

} // namespace Terrahertz

#endif // __linux__
#endif // !THZ_IMAGE_COMMON_DISPLAYSERVER_HPP

