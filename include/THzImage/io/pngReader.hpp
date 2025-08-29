#ifndef THZ_IMAGE_IO_PNGREADER_HPP
#define THZ_IMAGE_IO_PNGREADER_HPP

#include "THzCommon/utility/staticPImpl.hpp"
#include "THzImage/common/iImageReader.hpp"
#include "THzImage/common/pixel.hpp"

#include <cstdint>
#include <filesystem>

namespace Terrahertz::PNG {

/// @brief Reads an image from a file using the Portable-Network-Graphics format.
class Reader : public IImageReader<BGRAPixel>
{
public:
    using IImageReader::readInto;

    /// @brief Initializes a new PNG::Reader.
    ///
    /// @param filepath The path of the file to read from.
    Reader(std::filesystem::path const filepath) noexcept;

    /// @brief Explicitly deleted to prevent copy construction.
    Reader(Reader const &other) noexcept = delete;

    /// @brief Explicitly deleted to prevent move construction.
    Reader(Reader &&other) noexcept = delete;

    /// @brief Explicitly deleted to prevent copy assignment.
    Reader &operator=(Reader const &other) noexcept = delete;

    /// @brief Explicitly deleted to prevent move assignment.
    Reader &operator=(Reader &&other) noexcept = delete;

    /// @brief Finalizes this instance, performing a deinit.
    ~Reader() noexcept;

    /// @brief Checks if the given file can be read as a BMP file.
    ///
    /// @return True if the file can be read, false otherwise.
    bool fileTypeFits() noexcept;

    /// @copydoc IImageReader::imagePresent
    bool imagePresent() const noexcept override;

    /// @copydoc IImageReader::init
    bool init() noexcept override;

    /// @copydoc IImageReader::dimensions
    Rectangle dimensions() const noexcept override;

    /// @copydoc IImageReader::read
    bool read(gsl::span<BGRAPixel> buffer) noexcept override;

    /// @copydoc IImageReader::deinit
    void deinit() noexcept override;

private:
    /// @brief Forward declaration of the implementation.
    struct Impl;

    /// @brief Pointer to the implementation.
    StaticPImpl<Impl, 40U> _impl{};
};

} // namespace Terrahertz::PNG

#endif // !THZ_IMAGE_IO_PNGREADER_HPP
