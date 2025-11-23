#ifndef THZ_IMAGE_PROCESSING_READERLESSNODEBASE_HPP
#define THZ_IMAGE_PROCESSING_READERLESSNODEBASE_HPP

#include "THzImage/common/iImageReader.hpp"
#include "THzImage/common/pixel.hpp"
#include "THzImage/handling/imageRingBuffer.hpp"
#include "THzImage/processing/iNode.hpp"

#include <gsl/span>

namespace Terrahertz::ImageProcessing {

/// @brief A common base for all nodes that do not wrap a reader to work.
///
/// @tparam TPixelType The type of pixel output by this node.
template <Pixel TPixelType>
class ReaderlessNodeBase : public Internal::INode<TPixelType>
{
public:
    /// @brief
    using MyToCountResult = Internal::INode<TPixelType>::ToCountResult;

    ReaderlessNodeBase() noexcept = default;

    /// @copydoc INode::next
    [[nodiscard]] bool next() noexcept override { return false; }

    /// @copydoc INode::toCount
    [[nodiscard]] MyToCountResult toCount(size_t const target) noexcept override { return MyToCountResult::Failure; }

    /// @copydoc INode::operator[]
    [[nodiscard]] Image<TPixelType> &operator[](size_t const index) noexcept override { return _emptyImage; }

    /// @copydoc INode::slots
    [[nodiscard]] size_t slots() const noexcept override { return 0U; }

    /// @copydoc INode::count
    [[nodiscard]] size_t count() const noexcept override { return 0U; }

protected:
    virtual void process() noexcept = 0;

private:
    /// @brief Reader that does not change the image in the buffer.
    class NullReader : public IImageReader<TPixelType>
    {
    public:
        /// @copydoc IImageReader::imagePresent
        bool imagePresent() const noexcept override { return true; }

        /// @copydoc IImageReader::init
        bool init() noexcept override { return true; }

        /// @copydoc IImageReader::dimensions
        Rectangle dimensions() const noexcept override { return _dimensions; }

        /// @copydoc IImageReader::read
        bool read(gsl::span<TPixelType> buffer) noexcept override { return true; }

        /// @copydoc IImageReader::deinit
        void deinit() noexcept override {}

        /// @brief The dimensions for the image.
        Rectangle _dimensions{};
    };

    /// @brief The image returned if the operator[] gets and index out of range.
    Image<TPixelType> _emptyImage{};

    /// @brief The reader used by the buffer.
    NullReader _reader{};

    /// @brief The buffer used by the node.
    ImageRingBuffer<TPixelType> _buffer{};
};

} // namespace Terrahertz::ImageProcessing

#endif // !THZ_IMAGE_PROCESSING_READERLESSNODEBASE_HPP
