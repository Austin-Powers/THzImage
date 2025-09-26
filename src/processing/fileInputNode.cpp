#include "THzImage/processing/fileInputNode.hpp"

namespace Terrahertz::ImageProcessing {

FileInputNode::FileInputNode(size_t const bufferSize, std::filesystem::path const &path, Mode const mode) noexcept
    : _reader{path, mode}, _buffer{_reader, bufferSize}
{
    _paths.reserve(bufferSize);
}

std::filesystem::path const &FileInputNode::pathOf(size_t const index) const noexcept
{
    if (index < _pathMap.size())
    {
        return *(_pathMap[index]);
    }
    return _emptyPath;
}

bool FileInputNode::next() noexcept
{
    auto const result = _buffer.next();

    (*_pathMap[_pathMap.size() - 1U]) = _reader.pathOfLastImage();
    std::rotate(_pathMap.rbegin(), _pathMap.rbegin() + 1U, _pathMap.rend());

    return result;
}

FileInputNode::ImageType &FileInputNode::operator[](size_t const index) noexcept
{
    if (index < _buffer.slots())
    {
        return _buffer[index];
    }
    return _emptyImage;
}

} // namespace Terrahertz::ImageProcessing
