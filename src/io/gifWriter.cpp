#include "THzImage/io/gifWriter.hpp"

#include "THzCommon/utility/spanhelpers.hpp"
#include "THzImage/common/colorspaceconverter.hpp"

namespace Terrahertz::GIF {
namespace Internal {

constexpr size_t ColorReduction::TargetColors;

void ColorReduction::analyze(gsl::span<BGRAPixel const> const buffer) noexcept
{
    // build tree
    BGROctreeType tree{128U, 128U, 128U, 256U, static_cast<std::uint32_t>(buffer.size() / TargetColors), 8U};
    for (auto const &pixel : buffer)
    {
        tree.addEntry(pixel.blue, pixel.green, pixel.red, pixel.alpha);
    }
    tree.recalculate();

    // split tree to fill up  intermediate table
    std::array<BGROctreeType const *, TargetColors> nodes{};
    nodes[0U]       = &tree;
    auto emptySlots = nodes.size() - 1U;

    auto getLargestNodeIndex = [&nodes]() noexcept -> std::uint32_t {
        auto largestNodeIdx = 0U;
        for (auto i = 1U; nodes[i] != nullptr; ++i)
        {
            if (nodes[largestNodeIdx]->totalEntries() < nodes[i]->totalEntries())
            {
                largestNodeIdx = i;
            }
        }
        return largestNodeIdx;
    };
    auto getEmptyNodeIndex = [&nodes, &emptySlots]() noexcept -> std::uint32_t {
        for (auto i = 0U; i < nodes.size(); ++i)
        {
            if (nodes[i] == nullptr)
            {
                --emptySlots;
                return i;
            }
            if (nodes[i]->totalEntries() == 0U)
            {
                return i;
            }
        }
        return 0U;
    };

    while (emptySlots != 0U)
    {
        auto parentIdx = getLargestNodeIndex();
        auto children  = nodes[parentIdx]->getChildNodes();
        if (children.empty())
        {
            // if the largest node has no children, we have split all nodes
            break;
        }
        while (emptySlots != 0U)
        {
            if (children.size() > 1U)
            {
                nodes[getEmptyNodeIndex()] = &children[0U];
                children                   = children.subspan(1U);
            }
            else
            {
                nodes[parentIdx] = &children[0U];
                break;
            }
        }
    }

    // fill _colorTable from intermediate table
    // and update quick access tree
    struct Analyzer
    {
        size_t counter{};
        float  blue{};
        float  green{};
        float  red{};
        void   operator()(BGROctreeType::Entry const &entry) noexcept
        {
            ++counter;
            blue += entry.x;
            green += entry.y;
            red += entry.z;
        }
    };

    _colorCount = 0U;
    _quickAccess.reset(128U, 128U, 128U, 256U, 1U, 8U);
    for (auto const node : nodes)
    {
        if (node != nullptr)
        {
            auto    &slot = _colorTable[_colorCount];
            Analyzer anaylzer{};
            node->analyzeEntries(anaylzer);
            slot.blue  = static_cast<std::uint8_t>(anaylzer.blue / anaylzer.counter);
            slot.green = static_cast<std::uint8_t>(anaylzer.green / anaylzer.counter);
            slot.red   = static_cast<std::uint8_t>(anaylzer.red / anaylzer.counter);
            _quickAccess.addEntry(slot.blue, slot.green, slot.red, _colorCount);
            ++_colorCount;
        }
    }
}

gsl::span<BGRAPixel const> ColorReduction::colorTable() const noexcept
{
    return toSpan<BGRAPixel const>(_colorTable).subspan(0U, _colorCount);
}

std::uint8_t ColorReduction::convert(BGRAPixel const &color) const noexcept
{
    auto minDistance = color.distanceSquared(_colorTable[0U]);
    auto minIdx      = 0U;
    for (auto idx = 1U; idx < _colorCount; ++idx)
    {
        auto distance = color.distanceSquared(_colorTable[idx]);
        if (minDistance > distance)
        {
            minDistance = distance;
            minIdx      = idx;
        }
    }
    return minIdx;
}

} // namespace Internal

Writer::Writer(std::string_view const filepath) noexcept {}

bool Writer::init() noexcept { return false; }

bool Writer::write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept { return false; }

void Writer::deinit() noexcept {}

} // namespace Terrahertz::GIF
