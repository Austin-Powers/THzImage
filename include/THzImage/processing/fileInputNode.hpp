#ifndef THZ_IMAGE_PROCESSING_FILEINPUTNODE_HPP
#define THZ_IMAGE_PROCESSING_FILEINPUTNODE_HPP

#include "THzImage/handling/imageRingBuffer.hpp"
#include "THzImage/io/imageDirectoryReader.hpp"
#include "THzImage/processing/iNode.hpp"

#include <cstddef>
#include <filesystem>
#include <vector>

namespace Terrahertz::ImageProcessing {

/// @brief Node for loading a directory of images, not including sub-directories.
class FileInputNode : public Internal::INode<BGRAPixel>
{
public:
    /// @brief Shortcut to the mode enum from ImageDirectoryReader.
    using Mode = ImageDirectory::Reader::Mode;

    /// @brief Initializes a new FileInputNode.
    ///
    /// @param bufferSize The amount of images the node will store.
    /// @param path The path to the directory.
    /// @param mode The mode the node is operating in.
    FileInputNode(size_t const                 bufferSize,
                  std::filesystem::path const &path,
                  Mode const                   mode = Mode::automatic) noexcept;

    /// @brief Explicitly deleted to prevent copy construction.
    FileInputNode(FileInputNode const &) noexcept = delete;

    /// @brief Explicitly deleted to prevent move construction.
    FileInputNode(FileInputNode &&other) noexcept = delete;

    /// @brief Explicitly deleted to prevent copy assignment.
    FileInputNode &operator=(FileInputNode const &other) noexcept = delete;

    /// @brief Explicitly deleted to prevent move assignment.
    FileInputNode &operator=(FileInputNode &&other) noexcept = delete;

    /// @brief Finalizes this instance, performing a deinit.
    ~FileInputNode() noexcept = default;

    /// @brief Returns the path of the image at the given index in the buffer.
    ///
    /// @param index The index of the image.
    /// @return The path of the image.
    [[nodiscard]] std::filesystem::path const &pathOf(size_t const index) const noexcept;

    /// @brief Triggers loading the next image.
    ///
    /// @return True if the next image was loaded, false otherwise.
    [[nodiscard]] bool next() noexcept override;

    /// @copydoc INode::operator[]
    [[nodiscard]] ImageType &operator[](size_t const index) noexcept override;

    /// @copydoc INode::slots
    [[nodiscard]] size_t slots() const noexcept override;

private:
    /// @brief The image returned if operator[] gets an index out of range.
    ImageType _emptyImage{};

    /// @brief The path returned if pathOf() gets and index out of range.
    std::filesystem::path _emptyPath{};

    /// @brief The directory reader used to read the images.
    ImageDirectory::Reader _reader;

    /// @brief The buffer used by the node.
    ImageRingBuffer<BGRAPixel> _buffer;

    /// @brief The mapping of the paths newest to oldest entry.
    std::vector<std::filesystem::path *> _pathMap{};

    /// @brief Storage for the file paths corresponding to the images in the buffer.
    std::vector<std::filesystem::path> _paths{};
};

} // namespace Terrahertz::ImageProcessing

#endif // !THZ_IMAGE_PROCESSING_FILEINPUTNODE_HPP
