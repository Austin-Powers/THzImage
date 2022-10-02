set(PROJECTNAME THzImage.Tests)

add_executable(${PROJECTNAME}
	common/colorspaceconverter.cpp
	common/image.cpp
	common/pixel.cpp
)

target_include_directories(${PROJECTNAME} PUBLIC
	${PROJECT_SOURCE_DIR}
)

target_link_libraries(${PROJECTNAME} PUBLIC
	THzCommon
	THzImage
	gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECTNAME})

add_test(NAME ${PROJECTNAME} COMMAND ${PROJECTNAME})
