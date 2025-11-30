#include "THzImage/processing/fileInputNode.hpp"

namespace Terrahertz::ImageProcessing {

FileInputNode::FileInputNode(size_t const bufferSize, std::filesystem::path const &path, Mode const mode) noexcept
    : _reader{path, mode}, _buffer{_reader, bufferSize}
{
    _paths.resize(bufferSize);
    _pathMap.resize(bufferSize);
    for (auto i = 0U; i < bufferSize; ++i)
    {
        _pathMap[i] = &_paths[i];
    }
}

std::filesystem::path const &FileInputNode::pathOf(size_t const index) const noexcept
{
    if (index < _pathMap.size())
    {
        return *(_pathMap[index]);
    }
    return _emptyPath;
}

bool FileInputNode::next(bool const countFailure) noexcept
{
    auto const result = _buffer.next(countFailure);
    if (result)
    {
        (*_pathMap[_pathMap.size() - 1U]) = _reader.pathOfLastImage();
        std::rotate(_pathMap.rbegin(), _pathMap.rbegin() + 1U, _pathMap.rend());
    }
    return result;
}

ToCountResult FileInputNode::toCount(size_t const target, bool const force) noexcept
{
    if (target < _buffer.count())
    {
        return ToCountResult::Ahead;
    }
    if (target == _buffer.count())
    {
        return ToCountResult::NotUpdated;
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

FileInputNode::ImageType &FileInputNode::operator[](size_t const index) noexcept
{
    if (index < _buffer.slots())
    {
        return _buffer[index];
    }
    return _emptyImage;
}

size_t FileInputNode::slots() const noexcept { return _buffer.slots(); }

size_t FileInputNode::count() const noexcept { return _buffer.count(); }

} // namespace Terrahertz::ImageProcessing
