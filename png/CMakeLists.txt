# WHY: TL;DR CMake + Windows is a big NO NO when it comes to libpng
#
# On Windows it is expected you use the Microsoft Visual Studio Build System
# So the CMakeLists.txt is using some Linux-only-magic, which makes all of this less effort
# than trying to find a solution, if there even is one

include(FetchContent)

project(png VERSION 1.0)

FetchContent_Declare(
	zlib
	GIT_REPOSITORY https://github.com/madler/zlib.git
	GIT_TAG        09155eaa2f9270dc4ed1fa13e2b4b2613e6e4851 # release-v1.3
)
FetchContent_Declare(
	libpng
	GIT_REPOSITORY https://github.com/glennrp/libpng.git
	GIT_TAG        f135775ad4e5d4408d2e12ffcc71bb36e6b48551 # release-v1.6.40
)

FetchContent_MakeAvailable(zlib)
# Sadly there is no other way than doing things manually
FetchContent_Populate(libpng)

configure_file(
	${libpng_SOURCE_DIR}/scripts/pnglibconf.h.prebuilt
	${PROJECT_SOURCE_DIR}/pnglibconf.h
)

add_library(${PROJECT_NAME} STATIC
	${libpng_SOURCE_DIR}/png.c
	${libpng_SOURCE_DIR}/pngerror.c
	${libpng_SOURCE_DIR}/pngget.c
	${libpng_SOURCE_DIR}/pngmem.c
	${libpng_SOURCE_DIR}/pngpread.c
	${libpng_SOURCE_DIR}/pngread.c
	${libpng_SOURCE_DIR}/pngrio.c
	${libpng_SOURCE_DIR}/pngrtran.c
	${libpng_SOURCE_DIR}/pngrutil.c
	${libpng_SOURCE_DIR}/pngset.c
	${libpng_SOURCE_DIR}/pngtest.c
	${libpng_SOURCE_DIR}/pngtrans.c
	${libpng_SOURCE_DIR}/pngwio.c
	${libpng_SOURCE_DIR}/pngwrite.c
	${libpng_SOURCE_DIR}/pngwtran.c
	${libpng_SOURCE_DIR}/pngwutil.c

	${libpng_SOURCE_DIR}/png.h
	${libpng_SOURCE_DIR}/pngconf.h
	${libpng_SOURCE_DIR}/pngdebug.h
	${libpng_SOURCE_DIR}/pnginfo.h
	${libpng_SOURCE_DIR}/pngpriv.h
	${libpng_SOURCE_DIR}/pngstruct.h
	pnglibconf.h
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	# not my code, not my warnings, not my problem :)
	_CRT_SECURE_NO_WARNINGS
)

target_include_directories(${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR} # pnglibconf.h
	${libpng_SOURCE_DIR}  # the main files for libpng
	${zlib_SOURCE_DIR}    # the main files for zlib
	${zlib_BINARY_DIR}    # zconf.h
)

target_link_libraries(${PROJECT_NAME} PUBLIC
	zlibstatic
)
