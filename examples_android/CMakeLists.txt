cmake_minimum_required(VERSION 3.20)

project(xx2dExample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRCS)
file(GLOB SRCS	#GLOB_RECURSE
	${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../xx2d/*.h
	#...
)
add_library(${CMAKE_PROJECT_NAME} SHARED ${SRCS})

set(CMAKE_SHARED_LINKER_FLAGS
	"${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate"
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
	${ANDROID_NDK}/sources/android/native_app_glue
	${CMAKE_CURRENT_SOURCE_DIR}/../stb
	${CMAKE_CURRENT_SOURCE_DIR}/../pugixml/src
	${CMAKE_CURRENT_SOURCE_DIR}/../miniaudio
	${CMAKE_CURRENT_SOURCE_DIR}/../libvpx_prebuilt/includes
	${CMAKE_CURRENT_SOURCE_DIR}/../3rd
	${CMAKE_CURRENT_SOURCE_DIR}/../xxlib
	${CMAKE_CURRENT_SOURCE_DIR}/../src
	${CMAKE_CURRENT_SOURCE_DIR}/../spine-runtimes/spine-cpp/spine-cpp/include
	#...
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../spine-runtimes/spine-cpp spine-cpp)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../pugixml pugixml)

SET(BUILD_TESTING OFF CACHE BOOL "BUILD_TESTING" FORCE)
SET(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "USE_MSVC_RUNTIME_LIBRARY_DLL" FORCE)
SET(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "ZSTD_BUILD_PROGRAMS" FORCE)
SET(ZSTD_BUILD_SHARED OFF CACHE BOOL "ZSTD_BUILD_SHARED" FORCE)
SET(ZSTD_BUILD_TESTS OFF CACHE BOOL "ZSTD_BUILD_SHARED" FORCE)
SET(ZSTD_LEGACY_SUPPORT OFF CACHE BOOL "ZSTD_LEGACY_SUPPORT" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../zstd/build/cmake zstd)
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../zstd/lib
)


target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
	android
	EGL
	GLESv3
	log
#	android_native_app_glue
)
