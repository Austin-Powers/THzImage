#ifndef THZ_IMAGE_PROCESSING_SCREENINPUTNODE_HPP
#define THZ_IMAGE_PROCESSING_SCREENINPUTNODE_HPP

// functions used by this class are OS specific and we support only windows for now
#ifdef _WIN32

#include "THzCommon/math/rectangle.hpp"
#include "THzImage/common/pixel.hpp"
#include "THzImage/handling/imageRingBuffer.hpp"
#include "THzImage/io/screenReader.hpp"
#include "THzImage/processing/iNode.hpp"

namespace Terrahertz::ImageProcessing {

/// @brief Node for taking screenshots as an image source.
class ScreenInputNode : public Internal::INode<BGRAPixel>
{
public:
    /// @brief Initializes a new ScreenInputNode.
    ///
    /// @param bufferSize The amount of images the node will store.
    /// @remarks The node starts out making screenshots fullscreen, use setAreaTo() to change the area.
    ScreenInputNode(size_t const bufferSize) noexcept;

    /// @brief Explicitly deleted to prevent copy construction.
    ScreenInputNode(ScreenInputNode const &) noexcept = delete;

    /// @brief Explicitly deleted to prevent move construction.
    ScreenInputNode(ScreenInputNode &&other) noexcept = delete;

    /// @brief Explicitly deleted to prevent copy assignment.
    ScreenInputNode &operator=(ScreenInputNode const &other) noexcept = delete;

    /// @brief Explicitly deleted to prevent move assignment.
    ScreenInputNode &operator=(ScreenInputNode &&other) noexcept = delete;

    /// @brief Finalizes this instance, performing a deinit.
    ~ScreenInputNode() noexcept = default;

    /// @brief Sets the area screenshots are made from, to fullscreen.
    void setAreaToFullscreen() noexcept;

    /// @brief Sets the area screenshots are made from, to the given area.
    ///
    /// @param area The new area from which to make the screenshots from.
    /// @return True if the area was set, false in case the area does not fit on the screen.
    [[nodiscard]] bool setAreaTo(Rectangle const area) noexcept;

    /// @brief Triggers taking a new screenshot.
    ///
    /// @return True if the screenshot was taken, false otherwise.
    [[nodiscard]] bool next() noexcept override;

    /// @copydoc INode::operator[]
    [[nodiscard]] ImageType &operator[](size_t const index) noexcept override;

    /// @copydoc INode::slots
    [[nodiscard]] size_t slots() const noexcept override;

private:
    /// @brief The image returned if the operator[] gets an index out of range.
    ImageType _emptyImage{};

    /// @brief The screen reader used by the node.
    Screen::Reader _reader{};

    /// @brief The buffer used by the node.
    ImageRingBuffer<BGRAPixel> _buffer;
};

} // namespace Terrahertz::ImageProcessing

#endif // _WIN32
#endif // !THZ_IMAGE_PROCESSING_SCREENINPUTNODE_HPP
