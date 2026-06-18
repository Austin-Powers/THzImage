project('THzImage', 'cpp', version: '1.0.0', license: 'MIT')

include_dirs = include_directories('include')

sources = files(
    'src/analysis/basicImageMetrics.cpp',
	'src/common/pixel.cpp',
	'src/io/autoFileReader.cpp',
	'src/io/bmpReader.cpp',
	'src/io/bmpWriter.cpp',
	'src/io/imageDirectoryReader.cpp',
	'src/io/gifReader.cpp',
	'src/io/gifWriter.cpp',
	'src/io/pngReader.cpp',
	'src/io/pngWriter.cpp',
	'src/io/qoiReader.cpp',
	'src/io/qoiWriter.cpp',
	'src/io/screenReaderDummy.cpp',
	'src/io/screenReaderWayland.cpp',
	'src/io/screenReaderWin32.cpp',
	'src/io/screenReaderX11.cpp',
	'src/io/testImageGenerator.cpp',
	'src/io/bmpCommons.hpp',
	'src/io/gifCommons.hpp',
	'src/io/qoiCommons.hpp',
	'src/processing/dataReductionNode.cpp',
	'src/processing/easyWriter.cpp',
	'src/processing/fileInputNode.cpp',
	'src/processing/imageInputNode.cpp',
	'src/processing/screenInputNode.cpp',
	'src/processing/testInputNode.cpp',
)

gsl_proj = subproject('microsoft-gsl')
gsl_dep = gsl_proj.get_variable('microsoft_gsl_dep')

png_proj = subproject('libpng')
png_dep = png_proj.get_variable('libpng_dep')

thzcommon_proj = subproject('THzCommon')
thzcommon_dep = thzcommon_proj.get_variable('thzcommon_dep')

dependencies = [gsl_dep, png_dep, thzcommon_dep]

thzimage_lib = library(
    meson.project_name(),
    sources,
    include_directories: include_dirs,
    dependencies: dependencies,
    override_options: ['cpp_std=c++20'],
)

thzimage_dep = declare_dependency(include_directories: include_dirs, link_with: thzimage_lib)

test_sources = files(
    'test/analysis/basicImageMetrics.cpp',
	'test/common/colorspaceconverter.cpp',
	'test/common/image.cpp',
	'test/common/imageView.cpp',
	'test/common/pixel.cpp',
	'test/handling/asyncImageRingBuffer.cpp',
	'test/handling/bufferTransformer.cpp',
	'test/handling/imageRingBuffer.cpp',
	'test/io/asyncWriter.cpp',
	'test/io/autoFileReader.cpp',
	'test/io/bmpReader.cpp',
	'test/io/bmpWriter.cpp',
	'test/io/gifReader.cpp',
	'test/io/gifWriter.cpp',
	'test/io/imageDirectoryReader.cpp',
	'test/io/imageSeriesWriter.cpp',
	'test/io/pngReader.cpp',
	'test/io/pngWriter.cpp',
	'test/io/qoiReader.cpp',
	'test/io/qoiWriter.cpp',
	'test/io/screenReader.cpp',
	'test/io/testImageGenerator.cpp',
	'test/io/timedReader.cpp',
	'test/processing/dataReductionNode.cpp',
	'test/processing/easyWriter.cpp',
	'test/processing/fileInputNode.cpp',
	'test/processing/imageInputNode.cpp',
	'test/processing/readerlessNodeBase.cpp',
	'test/processing/testInputNode.cpp',
	'test/transformation/borderTransformer.cpp',
	'test/transformation/convolutionTransformerBase.cpp',
	'test/transformation/mockTransformer.hpp',
	'test/transformation/nullTransformer.cpp',
	'test/transformation/pixelTransformer.cpp',
	'test/sandbox.cpp',
)

gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_dep')
gmock_dep = gtest_proj.get_variable('gmock_main_dep')

test_deps = []
test_deps += dependencies
test_deps += gtest_dep
test_deps += gmock_dep

test_exe = executable(
	'THzImageTests',
	sources + test_sources,
	include_directories: include_dirs,
	dependencies: test_deps,
	override_options: ['cpp_std=c++20'],
)

test('THzImageTests', test_exe)