#ifndef THZ_IMAGE_IO_TIMEDREADER_HPP
#define THZ_IMAGE_IO_TIMEDREADER_HPP

#include "THzImage/common/iImageReader.hpp"

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <type_traits>

namespace Terrahertz {

/// @brief Wraps another reader class to add the ability to let time pass between reading cycles.
///
/// @tparam TWrapped The reader class to wrap.
template <ImageReader TWrapped>
class TimedReader : public IImageReader<typename TWrapped::PixelType>
{
public:
    using IImageReader<typename TWrapped::PixelType>::readInto;

    /// @brief The type of clock used by this class for time keeping.
    using Clock = std::chrono::steady_clock;

    /// @brief The type of time point used by this class for time keeping.
    using TimePoint = std::chrono::time_point<Clock>;

    /// @brief Structure containing information about the operation of the wrapper.
    struct Info
    {
        /// @brief The amount of time slots that passed without reading.
        std::uint32_t missedTimeSlots{};

        /// @brief The total amount of time slots that passed since setting the interval.
        std::uint32_t totalTimeSlots{};
    };

    /// @brief Initializes a new TimedReader using the given parameters.
    ///
    /// @tparam ...TParamTypes The types of the parameters for the constructor of the wrapped reader.
    /// @param pInterval The interval in which the reader cycle can be called.
    /// @param ...params The parameter for the constructor of the wrapped reader.
    /// @remark The timing works by delaying the init() call till the next time-point is reached.
    ///         Time-points are bound to the given interval.
    template <typename... TParamTypes>
    TimedReader(std::chrono::milliseconds const pInterval,
                TParamTypes &&...params) noexcept(std::is_nothrow_constructible_v<TWrapped, TParamTypes...>)
        : _wrapped{std::forward<TParamTypes>(params)...}
    {
        if (!setInterval(pInterval))
        {
            // will always return true
            (void)setInterval(std::chrono::milliseconds{1000});
        }
    }

    /// @brief Returns a reference to the wrapped reader, to provide access to special methods.
    ///
    /// @return A reference to the wrapped reader.
    TWrapped &wrappedReader() noexcept { return _wrapped; }

    /// @brief Returns the current interval.
    ///
    /// @return The current interval.
    std::chrono::milliseconds interval() const noexcept { return _interval; }

    /// @brief Sets the interval in which the reader cycle can be called.
    ///
    /// @param pInterval The interval in which the reader cycle can be called.
    /// @return True if the interval was updated, false otherwise (if pInterval is zero).
    [[nodiscard]] bool setInterval(std::chrono::milliseconds const &pInterval) noexcept
    {
        if (pInterval.count() <= 0)
        {
            return false;
        }
        _interval  = pInterval;
        _nextPoint = Clock::now() + _interval;
        _info      = Info{};
        return true;
    }

    /// @brief Returns inforamtion about the operation of the reader.
    ///
    /// @return Inforamtion about the operation of the reader.
    Info info() const noexcept { return _info; }

    /// @copydoc IImageReader::imagePresent
    bool imagePresent() const noexcept override { return _wrapped.imagePresent(); }

    /// @copydoc IImageReader::init
    bool init() noexcept override
    {
        std::unique_lock<std::mutex> lock{_mutex};

        auto const currentTime = Clock::now();
        while ((_nextPoint - currentTime).count() <= 0U)
        {
            _nextPoint += _interval;
            ++_info.missedTimeSlots;
            ++_info.totalTimeSlots;
        }
        _cv.wait_until(lock, _nextPoint);
        _nextPoint += _interval;
        ++_info.totalTimeSlots;
        return _wrapped.init();
    }

    /// @copydoc IImageReader::dimensions
    Rectangle dimensions() const noexcept override { return _wrapped.dimensions(); }

    /// @copydoc IImageReader::read
    bool read(gsl::span<typename TWrapped::PixelType> buffer) noexcept override { return _wrapped.read(buffer); }

    /// @copydoc IImageReader::deinit
    void deinit() noexcept override { _wrapped.deinit(); }

private:
    /// @brief The next point in time the reader cycle can be called.
    TimePoint _nextPoint{Clock::now()};

    /// @brief The interval in which the reader can be called.
    std::chrono::milliseconds _interval{};

    /// @brief The mutex for the condition variable.
    std::mutex _mutex{};

    /// @brief The condition variable to call wait_until on.
    std::condition_variable _cv{};

    /// @brief The wrapped reader.
    TWrapped _wrapped;

    /// @brief The info about the operation of the reader.
    Info _info{};
};

} // namespace Terrahertz

#endif // !THZ_IMAGE_IO_TIMEDREADER_HPP
