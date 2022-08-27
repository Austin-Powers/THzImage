cmake_minimum_required(VERSION 3.14)

if(NOT DEFINED PROJECT_NAME)
	set(THZ_IMAGE_IS_ROOT True)
else()
	set(THZ_IMAGE_IS_ROOT False)
endif()

project(THzImage VERSION 1.0)

include(cmake/helper_functions.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG        e2239ee6043f73722e7aa812a459f54a28552929 # release-1.11.0
)
FetchContent_Declare(
	gsl
	GIT_REPOSITORY https://github.com/microsoft/GSL.git
	GIT_TAG        a3534567187d2edc428efd3f13466ff75fe5805c # release-v4.0.0
)
FetchContent_MakeAvailable(gsl)

add_subdirectory(Common)

if(THZ_IMAGE_IS_ROOT)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	# Not needed here as there will be no parent project, but its good to remember this command
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
	add_subdirectory(Tests)
endif()

if(NOT DEFINED THzCommon)
	message("THzCommon not found")
endif()
