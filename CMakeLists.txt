cmake_minimum_required(VERSION 3.20)

set(name xx2d)
project(${name})

set(CMAKE_CXX_STANDARD 20)

include_directories(
	3rd
	xxlib
	src
	./
)

set(SRCS)
file(GLOB_RECURSE SRCS
	3rd/*.h
	xxlib/*.h
	src/*.cpp
	src/*.h
	src/*.hpp
	src/*.inc
)
add_executable(${name} ${SRCS})

if(MSVC)
	add_definitions("-D_SECURE_SCL=0")
	add_definitions("-D_HAS_ITERATOR_DEBUGGING=0")

	set_target_properties(${name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

	SET_TARGET_PROPERTIES(${name} PROPERTIES COMPILE_FLAGS "/GL")
	SET_TARGET_PROPERTIES(${name} PROPERTIES LINK_FLAGS "/LTCG")
	target_compile_options(${name} 
		PRIVATE $<$<PLATFORM_ID:Windows>:/bigobj>
		PRIVATE $<$<PLATFORM_ID:Windows>:/MP${Biogears_BUILD_PARALLEL_LEVEL}>)
else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
endif()


SET(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW_BUILD_DOCS" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW_BUILD_EXAMPLES" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW_BUILD_TESTS" FORCE)
SET(GLFW_INSTALL OFF CACHE BOOL "GLFW_INSTALL" FORCE)
add_subdirectory(glfw)
target_link_libraries(${name} glfw)


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
