#include "THzImage/processing/fileInputNode.hpp"

namespace Terrahertz::ImageProcessing {

FileInputNode::FileInputNode(size_t const bufferSize, std::filesystem::path const &path, Mode const mode) noexcept
    : _reader{path, mode}, _buffer{_reader, bufferSize}
{}

std::filesystem::path const &FileInputNode::pathOf(size_t const index) const noexcept { return _emptyPath; }

bool FileInputNode::next() noexcept { return false; }

FileInputNode::ImageType &FileInputNode::operator[](size_t const index) noexcept { return _emptyImage; }

} // namespace Terrahertz::ImageProcessing
