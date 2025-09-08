cmake_minimum_required(VERSION 3.14)
cmake_policy(VERSION 3.14)

if(NOT DEFINED PROJECT_NAME)
	set(THZ_IMAGE_IS_ROOT True)
else()
	set(THZ_IMAGE_IS_ROOT False)
endif()

project(THzImage VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG        e2239ee6043f73722e7aa812a459f54a28552929 # release-1.11.0
)
FetchContent_Declare(
	THzCommon
	GIT_REPOSITORY https://github.com/Austin-Powers/THzCommon
	GIT_TAG        71d874d06369b6944ca1f33e9c52aad6831c6411 # latest 25.06.25
)
FetchContent_Declare(
	gsl
	GIT_REPOSITORY https://github.com/microsoft/GSL.git
	GIT_TAG        a3534567187d2edc428efd3f13466ff75fe5805c # release-v4.0.0
)
FetchContent_MakeAvailable(gsl)
if(NOT DEFINED THzCommon_SOURCE_DIR)
	message("THzCommon not found...")
	message("Fetching THzCommon")
	FetchContent_MakeAvailable(THzCommon)
endif()

add_subdirectory(png)

add_library(${PROJECT_NAME} STATIC
	src/analysis/basicImageMetrics.cpp
	src/common/pixel.cpp
	src/io/autoFileReader.cpp
	src/io/bmpReader.cpp
	src/io/bmpWriter.cpp
	src/io/imageDirectoryReader.cpp
	src/io/gifReader.cpp
	src/io/gifWriter.cpp
	src/io/pngReader.cpp
	src/io/pngWriter.cpp
	src/io/qoiReader.cpp
	src/io/qoiWriter.cpp
	src/io/screenReader.cpp
	src/io/testImageGenerator.cpp
	src/io/bmpCommons.hpp
	src/io/gifCommons.hpp
	src/io/qoiCommons.hpp
	src/processing/fileInputNode.cpp
	src/processing/screenInputNode.cpp

	include/THzImage/analysis/basicImageMetrics.hpp
	include/THzImage/common/colorspaceconverter.hpp
	include/THzImage/common/iImageReader.hpp
	include/THzImage/common/iImageTransformer.hpp
	include/THzImage/common/iImageWriter.hpp
	include/THzImage/common/image.hpp
    include/THzImage/common/imageView.hpp
	include/THzImage/common/pixel.hpp
	include/THzImage/handling/asyncImageRingBuffer.hpp
	include/THzImage/handling/bufferTransformer.hpp
	include/THzImage/handling/imageRingBuffer.hpp
	include/THzImage/io/asyncWriter.hpp
	include/THzImage/io/autoFileReader.hpp
	include/THzImage/io/bmpReader.hpp
	include/THzImage/io/bmpWriter.hpp
	include/THzImage/io/gifReader.hpp
	include/THzImage/io/gifWriter.hpp
	include/THzImage/io/imageDirectoryReader.hpp
	include/THzImage/io/imageSeriesReader.hpp
	include/THzImage/io/imageSeriesWriter.hpp
	include/THzImage/io/pngReader.hpp
	include/THzImage/io/pngWriter.hpp
	include/THzImage/io/qoiReader.hpp
	include/THzImage/io/qoiWriter.hpp
	include/THzImage/io/screenReader.hpp
	include/THzImage/io/testImageGenerator.hpp
	include/THzImage/io/timedReader.hpp
	include/THzImage/processing/iNode.hpp
	include/THzImage/processing/fileInputNode.hpp
	include/THzImage/processing/screenInputNode.hpp
	include/THzImage/transformation/borderTransformer.hpp
	include/THzImage/transformation/convolutionTransformerBase.hpp
	include/THzImage/transformation/nullTransformer.hpp
	include/THzImage/transformation/pixelTransformer.hpp
	include/THzImage/thzImage.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC
	Microsoft.GSL::GSL
	THzCommon
	png
)

if(THZ_IMAGE_IS_ROOT OR BUILD_THZ_IMAGE_TESTS)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	# Not needed here as there will be no parent project, but its good to remember this command
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
	enable_testing()
	add_subdirectory(test)
endif()
