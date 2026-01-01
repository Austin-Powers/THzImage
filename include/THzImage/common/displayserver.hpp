#ifndef THZ_IMAGE_COMMON_DISPLAYSERVER_HPP
#define THZ_IMAGE_COMMON_DISPLAYSERVER_HPP
#ifdef __linux__

// Switch used to decide witch display server is used by the system
// and should be used by the programms using THzImage

#if defined(USING_X11)
#define THZ_IMAGE_X11_SCREENREADER_USED
#elif defined(USING_WAYLAND)
#define THZ_IMAGE_WAYLAND_SCREENREADER_USED
#else
#define THZ_IMAGE_DUMMY_SCREENREADER_USED
#endif // USING_X11

#endif // __linux__
#endif // !THZ_IMAGE_COMMON_DISPLAYSERVER_HPP

