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
class ReaderlessNodeBase : public INode<TPixelType>, IImageReader<TPixelType>
{
public:
    using MyImageType = Image<TPixelType>;

    /// @brief Initializes a new readerless node.
    ///
    /// @param bufferSize The amount of images the node will store.
    ReaderlessNodeBase(size_t const bufferSize) noexcept : _buffer{*this, bufferSize} {}

    // INode methods
    /// @copydoc INode::next
    [[nodiscard]] bool next(bool const countFailure = false) noexcept override { return _buffer.next(countFailure); }

    /// @copydoc INode::toCount
    [[nodiscard]] ToCountResult toCount(size_t const target, bool const force = false) noexcept override
    {
        if (target < _buffer.count())
        {
            return ToCountResult::Ahead;
        }
        if (target == _buffer.count())
        {
            return ToCountResult::NotUpdated;
        }
        auto const gap = target - _buffer.count();
        if (gap > _buffer.slots())
        {
            for (auto i = gap - _buffer.slots(); i != 0U; --i)
            {
                _buffer.skip();
            }
        }
        while (target > _buffer.count())
        {
            if (!next(force) && !force)
            {
                return ToCountResult::Failure;
            }
        }
        return ToCountResult::Updated;
    }

    /// @copydoc INode::operator[]
    [[nodiscard]] MyImageType &operator[](size_t const index) noexcept override
    {
        if (index < _buffer.slots())
        {
            return _buffer[index];
        }
        return _emptyImage;
    }

    /// @copydoc INode::slots
    [[nodiscard]] size_t slots() const noexcept override { return _buffer.slots(); }

    /// @copydoc INode::count
    [[nodiscard]] size_t count() const noexcept override { return _buffer.count(); }

    // IImageReader methods
    /// @copydoc IImageReader::imagePresent
    bool imagePresent() const noexcept override { return true; }

    /// @copydoc IImageReader::init
    bool init() noexcept override { return prepareProcessing(_buffer.count() + 1U); }

    /// @copydoc IImageReader::dimensions
    Rectangle dimensions() const noexcept override { return dimensionsOfNextImage(); }

    /// @copydoc IImageReader::read
    bool read(gsl::span<TPixelType> buffer) noexcept override { return runProcessing(buffer); }

    /// @copydoc IImageReader::deinit
    void deinit() noexcept override {}

protected:
    /// @brief Is Called in preparation for processing the next image.
    ///
    /// @param count The count of the processed image.
    /// @return True if the class is able to process the next image, false otherwise.
    virtual bool prepareProcessing(size_t const count) noexcept = 0;

    /// @brief Retrieves the dimensions of the next image.
    ///
    /// @return The dimensions of the next image.
    virtual Rectangle dimensionsOfNextImage() const noexcept = 0;

    /// @brief Hands in the buffer of the next image to store the processing result.
    ///
    /// @param buffer The buffer to store the processing result.
    /// @return True if processing was succesful, false otherwise.
    virtual bool runProcessing(gsl::span<TPixelType> buffer) noexcept = 0;

private:
    /// @brief The image returned if the operator[] gets and index out of range.
    Image<TPixelType> _emptyImage{};

    /// @brief The buffer used by the node.
    ImageRingBuffer<TPixelType> _buffer;
};

} // namespace Terrahertz::ImageProcessing

#endif // !THZ_IMAGE_PROCESSING_READERLESSNODEBASE_HPP
