set(PROJECTNAME THzImage.Tests)

add_executable(${PROJECTNAME}
	analysis/basicImageMetrics.cpp
	common/colorspaceconverter.cpp
	common/image.cpp
	common/imageView.cpp
	common/pixel.cpp
	handling/asyncImageRingBuffer.cpp
	handling/imageRingBuffer.cpp
	io/asyncWriter.cpp
	io/bmpReader.cpp
	io/bmpWriter.cpp
	io/gifReader.cpp
	io/gifWriter.cpp
	io/imageSeriesReader.cpp
	io/imageSeriesWriter.cpp
	io/pngReader.cpp
	io/pngWriter.cpp
	io/qoiReader.cpp
	io/qoiWriter.cpp
	io/screenReader.cpp
	io/testImageGenerator.cpp
	io/timedReader.cpp
	transformation/pixelTransformer.cpp
	sandbox.cpp
)

target_include_directories(${PROJECTNAME} PUBLIC
	${PROJECT_SOURCE_DIR}
)

target_link_libraries(${PROJECTNAME} PUBLIC
	THzCommon
	THzImage
	gmock_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECTNAME})

add_test(NAME ${PROJECTNAME} COMMAND ${PROJECTNAME})
