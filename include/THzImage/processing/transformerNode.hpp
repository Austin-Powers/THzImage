#ifndef THZ_IMAGE_PROCESSING_TRANSFORMERNODE_HPP
#define THZ_IMAGE_PROCESSING_TRANSFORMERNODE_HPP

#include "THzImage/common/iImageTransformer.hpp"
#include "THzImage/common/pixel.hpp"
#include "THzImage/handling/imageRingBuffer.hpp"
#include "THzImage/processing/iNode.hpp"

namespace Terrahertz::ImageProcessing::Internal {

template <Pixel TPixelType, ImageTransformer<TPixelType> TTransformerType>
class TransformerNode : public INode<TPixelType>
{
public:
    /// @brief Initializes a new TransformerNode using the given parameters.
    ///
    /// @tparam ...TParamTypes The types of the parameters for the constructor of the wrapped transformer.
    /// @param bufferSize The amount of images the node will store.
    /// @param forwardNext Flag signalling if the next() call shall be forwarded to the wrapped node.
    /// @param ...params The parameters for the constructor of the wrapped transformer.
    template <typename... TParamTypes>
    TransformerNode(size_t const bufferSize,
                    bool         forwardNext,
                    TParamTypes &&...params) noexcept(std::is_nothrow_constructible_v<TTransformerType, TParamTypes...>)
        : _transformer{std::forward<TParamTypes>(params)...}, _buffer{_transformer, bufferSize, forwardNext}
    {}

    /// @brief Triggers loading the next image.
    ///
    /// @return True if the next image was loaded, false otherwise.
    [[nodiscard]] bool next() noexcept override { return _buffer.next(); }

    /// @copydoc INode::operator[]
    [[nodiscard]] Image<TPixelType> &operator[](size_t const index) noexcept override
    {
        if (index < _buffer.slots())
        {
            return _buffer[index];
        }
        return _emptyImage;
    }

    /// @copydoc INode::slots
    [[nodiscard]] size_t slots() const noexcept override { return _buffer.slots(); }

private:
    /// @brief The image returned if operator[] gets an index out of range.
    Image<TPixelType> _emptyImage{};

    /// @brief The wrapped transformer.
    TTransformerType _transformer;

    /// @brief The buffer used by the node.
    ImageRingBuffer<TPixelType> _buffer;
};

} // namespace Terrahertz::ImageProcessing::Internal

#endif // !THZ_IMAGE_PROCESSING_TRANSFORMERNODE_HPP
