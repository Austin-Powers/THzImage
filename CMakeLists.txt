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
	GIT_TAG        d5d9541e28af401fe8be5f5f7888146a4399eb28 # latest 22.09.12
)
FetchContent_Declare(
	gsl
	GIT_REPOSITORY https://github.com/microsoft/GSL.git
	GIT_TAG        a3534567187d2edc428efd3f13466ff75fe5805c # release-v4.0.0
)
FetchContent_MakeAvailable(gsl)
if(NOT DEFINED THzCommon)
	message("THzCommon not found...")
	message("Fetching THzCommon")
	FetchContent_MakeAvailable(THzCommon)
endif()

add_library(${PROJECT_NAME} STATIC
	src/common/pixel.cpp

	include/THzImage/common/colorspaceconverter.h
	include/THzImage/common/pixel.h
)

target_include_directories(${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC
	Microsoft.GSL::GSL
	THzCommon
)

if(THZ_IMAGE_IS_ROOT)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	# Not needed here as there will be no parent project, but its good to remember this command
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
	add_subdirectory(test)
endif()
