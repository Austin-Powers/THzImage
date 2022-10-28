#include "THzImage/io/qoiWriter.h"

#include <gtest/gtest.h>

namespace Terrahertz::UnitTests {

struct IO_QOIWriter : public testing::Test
{
    QOI::Internal::Compressor compressor{};
};

} // namespace Terrahertz::UnitTests
