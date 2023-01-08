#include "THzImage/io/gifWriter.hpp"

#include "THzCommon/structures/octree.hpp"
#include "THzCommon/utility/spanhelpers.hpp"
#include "THzImage/common/colorspaceconverter.hpp"

#include <algorithm>

namespace Terrahertz::GIF {
namespace Internal {

/// @brief The type of octree to reduction is based on.
using BGROctreeType = Octree<std::uint8_t, std::uint16_t, std::uint8_t>;

constexpr size_t ColorReduction::TargetColors;
constexpr size_t ColorReduction::QALength;
constexpr size_t ColorReduction::QAEntries;
constexpr size_t ColorReduction::QASlice;

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
            // TODO assumption incorrect largest node can have no children while others still have
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
    _colorCount = 0U;
    for (auto const node : nodes)
    {
        if (node != nullptr)
        {
            size_t     counter{};
            float      blue{};
            float      green{};
            float      red{};
            auto      &slot     = _colorTable[_colorCount];
            auto const anaylzer = [&](BGROctreeType::Entry const &entry) noexcept {
                ++counter;
                blue += entry.x;
                green += entry.y;
                red += entry.z;
            };
            node->analyzeEntries(anaylzer);
            slot.blue  = static_cast<std::uint8_t>(blue / counter);
            slot.green = static_cast<std::uint8_t>(green / counter);
            slot.red   = static_cast<std::uint8_t>(red / counter);
            ++_colorCount;
        }
    }

    // update quick access
    auto const updateCell = [this](std::array<std::uint8_t, QAEntries> &cell,
                                   std::uint32_t const                  b,
                                   std::uint32_t const                  g,
                                   std::uint32_t const                  r) noexcept {
        auto const startValue = QASlice / 2U;
        std::fill(cell.begin(), cell.end(), 0U);
        std::array<size_t, QAEntries> distances{};

        BGRAPixel const cellColor{static_cast<std::uint8_t>(startValue + (b * QASlice)),
                                  static_cast<std::uint8_t>(startValue + (g * QASlice)),
                                  static_cast<std::uint8_t>(startValue + (r * QASlice))};
        for (auto i = 0U; i < distances.size(); ++i)
        {
            if (_colorCount == i)
            {
                return;
            }
            cell[i]      = i;
            distances[i] = cellColor.distanceSquared(_colorTable[i]);
        }
        for (auto i = distances.size(); i < _colorCount; ++i)
        {
            auto greatestDistanceIdx = 0U;
            auto greatestDistance    = distances[greatestDistanceIdx];
            for (auto j = 1U; j < distances.size(); ++j)
            {
                if (greatestDistance < distances[j])
                {
                    greatestDistanceIdx = j;
                    greatestDistance    = distances[j];
                }
            }
            auto const distance = cellColor.distanceSquared(_colorTable[i]);
            if (distance < greatestDistance)
            {
                cell[greatestDistanceIdx]      = static_cast<std::uint8_t>(i);
                distances[greatestDistanceIdx] = distance;
            }
        }
    };
    for (auto b = 0U; b < _quickAccess.size(); ++b)
    {
        for (auto g = 0U; g < _quickAccess[b].size(); ++g)
        {
            for (auto r = 0U; r < _quickAccess[b][g].size(); ++r)
            {
                updateCell(_quickAccess[b][g][r], b, g, r);
            }
        }
    }
}

gsl::span<BGRAPixel const> ColorReduction::colorTable() const noexcept
{
    return toSpan<BGRAPixel const>(_colorTable).subspan(0U, _colorCount);
}

std::uint8_t ColorReduction::convert(BGRAPixel const &color) const noexcept
{
    auto minIdx      = 0U;
    auto minDistance = 256ULL * 256U * 4U;
    for (auto const idx : _quickAccess[color.blue / QASlice][color.green / QASlice][color.red / QASlice])
    {
        auto const distance = color.distanceSquared(_colorTable[idx]);
        if (minDistance > distance)
        {
            minDistance = distance;
            minIdx      = idx;
        }
    }
    return minIdx;
}

void Dithering::setParameters(std::uint32_t const width, ColorReduction const &colorReduction) noexcept
{
    _width          = width;
    _rowCounter     = 0U;
    _colorReduction = &colorReduction;
    _floatingParts.resize(1ULL + width);
    std::fill(_floatingParts.begin(), _floatingParts.end(), BGRAPixelFloat{});
    _pixel[0U] = _floatingParts.data();
    _pixel[1U] = _floatingParts.data() + 1;
    _pixel[2U] = _floatingParts.data() + _floatingParts.size() - 3U;
    _pixel[3U] = _floatingParts.data() + _floatingParts.size() - 2U;
    _pixel[4U] = _floatingParts.data() + _floatingParts.size() - 1U;
    _pixel[5U] = _floatingParts.data() + _floatingParts.size();
}

std::uint8_t Dithering::convert(BGRAPixel color) noexcept
{
    if (_colorReduction == nullptr)
    {
        return 0U;
    }

    auto const addErrorToColor = [](float error, uint8_t &color) -> float {
        error = std::clamp(error + color, 0.0f, 255.0f);
        color = static_cast<uint8_t>(error);
        return error;
    };

    // Add error to current color
    _pixel[0]->blue  = addErrorToColor(_pixel[0]->blue, color.blue);
    _pixel[0]->green = addErrorToColor(_pixel[0]->green, color.green);
    _pixel[0]->red   = addErrorToColor(_pixel[0]->red, color.red);
    _pixel[0]->alpha = addErrorToColor(_pixel[0]->alpha, color.alpha);

    auto const  result      = _colorReduction->convert(color);
    auto const &resultPixel = _colorReduction->colorTable()[result];

    // calculate error
    BGRAPixelFloat const error{(_pixel[0]->blue - resultPixel.blue) / 16.0f,
                               (_pixel[0]->green - resultPixel.green) / 16.0f,
                               (_pixel[0]->red - resultPixel.red) / 16.0f,
                               (_pixel[0]->alpha - resultPixel.alpha) / 16.0f};

    auto const applyError = [this, &error](size_t idx, float factor) {
        _pixel[idx]->blue += error.blue * factor;
        _pixel[idx]->green += error.green * factor;
        _pixel[idx]->red += error.red * factor;
        _pixel[idx]->alpha += error.alpha * factor;
    };

    // do not apply error on the borders
    if (_rowCounter != _width - 1)
    {
        applyError(1, 7.0f);
        applyError(4, 1.0f);
    }
    if (_rowCounter != 0)
    {
        applyError(2, 3.0f);
    }
    applyError(3, 5.0f);
    ++_rowCounter;
    if (_rowCounter == _width)
    {
        _rowCounter = 0;
    }

    _pixel[0]->blue   = 0.0f;
    _pixel[0]->green  = 0.0f;
    _pixel[0]->red    = 0.0f;
    _pixel[0]->alpha  = 0.0f;
    auto const moveOn = [this](size_t idx) {
        ++(_pixel[idx]);
        if (_pixel[idx] == _pixel[5])
        {
            _pixel[idx] = _floatingParts.data();
        }
    };
    moveOn(0);
    moveOn(1);
    moveOn(2);
    moveOn(3);
    moveOn(4);
    return result;
}

} // namespace Internal

Writer::Writer(std::string_view const filepath) noexcept {}

bool Writer::init() noexcept { return false; }

bool Writer::write(Rectangle const &dimensions, gsl::span<BGRAPixel const> const buffer) noexcept { return false; }

void Writer::deinit() noexcept {}

} // namespace Terrahertz::GIF
