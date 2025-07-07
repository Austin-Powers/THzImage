#ifdef _WIN32

#include "THzImage/io/screenReader.hpp"

#include "THzCommon/logging/logging.hpp"
#include "THzCommon/utility/flipBuffer.hpp"
#include "bmpCommons.hpp"

#include <Windows.h>
#include <cstdint>

namespace Terrahertz::Screen {

/// @brief Name provider for the THzImage.IO.ScreenReader class.
struct ReaderProject
{
    static constexpr char const *name() noexcept { return "THzImage.IO.ScreenReader"; }
};

Rectangle Reader::getScreenDimensions() noexcept
{
    return Rectangle{static_cast<std::uint32_t>(GetSystemMetrics(SM_CXSCREEN)),
                     static_cast<std::uint32_t>(GetSystemMetrics(SM_CYSCREEN))};
}

/// @brief Implementation of the Reader.
struct Reader::Impl
{
    /// @brief Initializes the implementation of the Screen::Reader class.
    ///
    /// @param area The area of the Screen the Reader shall capture.
    /// @remarks If The given area exceeds the screen, the screenDimensions will be used instead.
    Impl(Rectangle const &area) noexcept
    {
        Logger::globalInstance().addProject<ReaderProject>();
        if (!setArea(area))
        {
            logMessage<LogLevel::Warning, ReaderProject>(
                "Construction using invalid area, defaulting to screen dimensions");
            if (!setArea(Screen::Reader::getScreenDimensions()))
            {
                logMessage<LogLevel::Error, ReaderProject>("Defaulting to screen dimensions failed");
            }
        }
    }

    /// @brief Sets the area of the screen the reader captures.
    ///
    /// @param area The area of the screen the reader captures, needs to be inside the screen.
    /// @return True if the area has been set, false otherwise.
    [[nodiscard]] bool setArea(Rectangle const &area) noexcept
    {
        auto const check = Screen::Reader::getScreenDimensions().intersection(area);
        if (area != check)
        {
            logMessage<LogLevel::Error, ReaderProject>("Given area outside the screen");
            return false;
        }

        _area = area;
        return true;
    }

    /// @brief Returns the area on the screen the reader captures.
    ///
    /// @return The area on the screen the reader captures.
    [[nodiscard]] Rectangle area() const noexcept { return _area; }

    /// @brief Reads the current state of the area of the screen.
    ///
    /// @param buffer The buffer to store the image data in.
    /// @return True on successful reading, false otherwise.
    [[nodiscard]] bool read(gsl::span<BGRAPixel> buffer) noexcept
    {
        if (buffer.size() < _area.area())
        {
            logMessage<LogLevel::Error, ReaderProject>("Given buffer too small for image-data");
            return false;
        }

        auto const handleError = [this](char const message[]) noexcept -> bool {
            logMessage<LogLevel::Error, ReaderProject>(message);
            releaseHandles();
            return false;
        };

        // TODO THz check if cretain handles can be hold for longer and if it increases performance to do so
        // get device context for the entire screen
        _hScreenDC = GetDC(nullptr);
        if (!_hScreenDC)
        {
            return handleError("Unable to get device context for the screen");
        }
        // create device context compatible with the current screen of the application
        _hMemoryDC = CreateCompatibleDC(nullptr);
        if (!_hMemoryDC)
        {
            return handleError("Unable to create memory device context");
        }
        // creates a bitmap compatible with the screen device context
        _hScreenBM = CreateCompatibleBitmap(_hScreenDC, _area.width, _area.height);
        if (!_hScreenBM)
        {
            return handleError("Unable to create compatible bitmap");
        }

        SelectObject(_hMemoryDC, _hScreenBM);
        if (!BitBlt(_hMemoryDC,
                    0,
                    0,
                    _area.width,
                    _area.height,
                    _hScreenDC,
                    _area.upperLeftPoint.x,
                    _area.upperLeftPoint.y,
                    SRCCOPY))
        {
            return handleError("Bit-Block-Transfer failed");
        }

        BITMAP bmpScreen;
        // get data about the screen bitmap
        GetObject(_hScreenBM, sizeof(BITMAP), &bmpScreen);

        BMP::Header header{
            gsl::narrow_cast<std::int32_t>(_area.width), gsl::narrow_cast<std::int32_t>(_area.height), 32U};
        // copy bits from the compatible bitmap into the buffer
        GetDIBits(_hScreenDC,
                  _hScreenBM,
                  0,
                  static_cast<UINT>(bmpScreen.bmHeight),
                  reinterpret_cast<char *>(buffer.data()),
                  reinterpret_cast<BITMAPINFO *>(&header.infoHeader),
                  DIB_RGB_COLORS);

        releaseHandles();
        return flipBufferHorizontally(buffer, _area);
    }

    /// @brief Releases all handles needed for taken a screenshot.
    void releaseHandles() noexcept
    {
        if (_hScreenBM != nullptr)
        {
            DeleteObject(_hScreenBM);
        }
        if (_hMemoryDC != nullptr)
        {
            DeleteObject(_hMemoryDC);
        }
        if (_hScreenDC != nullptr)
        {
            ReleaseDC(nullptr, _hScreenDC);
        }
    }

    /// @brief The area on the screen the reader captures.
    Rectangle _area{};

    /// @brief The device context for the screen.
    HDC _hScreenDC{};

    /// @brief device context compatible with the current screen of the application.
    HDC _hMemoryDC{};

    /// @brief The bitmap compatible with the screen device context.
    HBITMAP _hScreenBM{};
};

Reader::Reader() noexcept { _impl.init(getScreenDimensions()); }

Reader::Reader(Rectangle const &area) noexcept { _impl.init(area); }

Reader::~Reader() noexcept {}

bool Reader::setArea(Rectangle const &area) noexcept { return _impl->setArea(area); }

bool Reader::imagePresent() const noexcept { return true; }

bool Reader::init() noexcept { return true; }

Rectangle Reader::dimensions() const noexcept { return _impl->area(); }

bool Reader::read(gsl::span<BGRAPixel> buffer) noexcept { return _impl->read(buffer); }

void Reader::deinit() noexcept {}

} // namespace Terrahertz::Screen

#endif // _WIN32
