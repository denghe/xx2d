cmake_minimum_required(VERSION 3.20)

set(name xx2d)
project(${name})

option(XX2D_ENABLE_IMGUI "Enable imgui support" ON)
if (XX2D_ENABLE_IMGUI)
	add_definitions("-DXX2D_ENABLE_IMGUI")
endif()

option(XX2D_ENABLE_MINIAUDIO "Enable miniaudio support" ON)
if (XX2D_ENABLE_MINIAUDIO)
	add_definitions("-DXX2D_ENABLE_MINIAUDIO")
endif()

set(CMAKE_CXX_STANDARD 20)

include_directories(
	examples
	glad
	stb
	imgui
	imgui/backends
	miniaudio
	xxlib
	src
)

set(SRCS)
file(GLOB SRCS	#GLOB_RECURSE
	glad/*.h
	stb/*.h
	miniaudio/*.h
	xxlib/*.h
	src/*.cpp
	src/*.h
	src/*.hpp
	src/*.inc
	examples/*.h
	examples/*.hpp
	examples/*.inc
	examples/*.c
	examples/*.cpp
)
add_executable(${name} ${SRCS})

source_group("" FILES ${SRCS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRCS})


if(MSVC)

	add_compile_options(
		$<$<CONFIG:MinSizeRel>:/MT>
		$<$<CONFIG:Release>:/MT>
		$<$<CONFIG:RelWithDebInfo>:/MT>
	)

	add_definitions("-D_SECURE_SCL=0")
	add_definitions("-D_HAS_ITERATOR_DEBUGGING=0")
	add_compile_options($<$<PLATFORM_ID:Windows>:/MP${Biogears_BUILD_PARALLEL_LEVEL}>)	# other proj enable multi-processor compilation

	target_compile_options(${name}
		PRIVATE $<$<PLATFORM_ID:Windows>:/MT>
		PRIVATE $<$<PLATFORM_ID:Windows>:/bigobj>
		PRIVATE $<$<PLATFORM_ID:Windows>:/MP${Biogears_BUILD_PARALLEL_LEVEL}>	# this proj enable multi-processor compilation
		PRIVATE $<$<CONFIG:MinSizeRel>:/GL>
		PRIVATE $<$<CONFIG:Release>:/GL>
		PRIVATE $<$<CONFIG:RelWithDebInfo>:/GL>
		PRIVATE $<$<PLATFORM_ID:Windows>:/Yuxx2d_pch.h>	# use pre-compile header
	)
	target_link_options(${name}
		PRIVATE $<$<CONFIG:MinSizeRel>:/LTCG>
		PRIVATE $<$<CONFIG:Release>:/LTCG>
		PRIVATE $<$<CONFIG:RelWithDebInfo>:/LTCG>
	)

	set_source_files_properties(src/xx2d_pch.cpp PROPERTIES COMPILE_FLAGS "/Ycxx2d_pch.h")		# make pre-compile header

	set_target_properties(${name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

#else()
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")	# todo: only for release?
endif()


SET(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW_BUILD_DOCS" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW_BUILD_EXAMPLES" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW_BUILD_TESTS" FORCE)
SET(GLFW_INSTALL OFF CACHE BOOL "GLFW_INSTALL" FORCE)
add_subdirectory(glfw)
target_link_libraries(${name} glfw)


add_subdirectory(imgui_cmake imgui)
target_link_libraries(${name} imgui)


add_subdirectory(pugixml)
target_link_libraries(${name} pugixml)


SET(BUILD_TESTING OFF CACHE BOOL "BUILD_TESTING" FORCE)
SET(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "USE_MSVC_RUNTIME_LIBRARY_DLL" FORCE)
SET(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "ZSTD_BUILD_PROGRAMS" FORCE)
SET(ZSTD_BUILD_SHARED OFF CACHE BOOL "ZSTD_BUILD_SHARED" FORCE)
SET(ZSTD_BUILD_TESTS OFF CACHE BOOL "ZSTD_BUILD_SHARED" FORCE)
SET(ZSTD_LEGACY_SUPPORT OFF CACHE BOOL "ZSTD_LEGACY_SUPPORT" FORCE)
add_subdirectory(zstd/build/cmake zstd)
target_link_libraries(${name} libzstd_static)
include_directories(
	zstd/lib
)
