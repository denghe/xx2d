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
	glad
	stb
	glfw/include
	pugixml/src
	imgui
	imgui/backends
	miniaudio
	3rd
	xxlib
	src
)

set(SRCS)
file(GLOB SRCS	#GLOB_RECURSE
	glad/*.h
	stb/*.h
	glfw/include/GLFW/*.h
	pugixml/src/*.hpp
	miniaudio/*.h
	3rd/*.*
	xxlib/*.h
	src/*.cpp
	src/*.h
	src/*.hpp
	src/*.inc
)
add_library(${name} ${SRCS})

source_group("" FILES ${SRCS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRCS})


if(MSVC)	# vs2022+

#	option(XX2D_ENABLE_STATIC_LINK "Enable static link" OFF)
#	if (XX2D_ENABLE_STATIC_LINK)
#		add_compile_options(
#			$<$<CONFIG:MinSizeRel>:/MT>
#			$<$<CONFIG:Release>:/MT>
#			$<$<CONFIG:RelWithDebInfo>:/MT>
#		)
#		target_compile_options(${name}
#			PRIVATE $<$<CONFIG:Release>:/MT>
#		)
#	endif()

#	target_link_options(${name}
#		PRIVATE $<$<CONFIG:MinSizeRel>:/LTCG>
#		PRIVATE $<$<CONFIG:Release>:/LTCG>
#		PRIVATE $<$<CONFIG:RelWithDebInfo>:/LTCG>
#	)

	target_link_options(${name}
		PRIVATE $<$<CONFIG:Debug>:/NODEFAULTLIB:libcmtd.lib>
		PRIVATE $<$<CONFIG:MinSizeRel>:/NODEFAULTLIB:libcmt.lib>
		PRIVATE $<$<CONFIG:Release>:/NODEFAULTLIB:libcmt.lib>
		PRIVATE $<$<CONFIG:RelWithDebInfo>:/NODEFAULTLIB:libcmt.lib>
	)

	set_source_files_properties(src/xx2d.cpp PROPERTIES COMPILE_FLAGS "/Ycxx2d.h")
	target_compile_options(${name} PRIVATE $<$<PLATFORM_ID:Windows>:/Yuxx2d.h>)


	target_compile_definitions(${name} PUBLIC
		"$<$<CONFIG:MinSizeRel>:_SECURE_SCL=0>"
		"$<$<CONFIG:MinSizeRel>:_HAS_ITERATOR_DEBUGGING=0>"
		"$<$<CONFIG:Release>:_SECURE_SCL=0>"
		"$<$<CONFIG:Release>:_HAS_ITERATOR_DEBUGGING=0>"
		"$<$<CONFIG:RelWithDebInfo>:_SECURE_SCL=0>"
		"$<$<CONFIG:RelWithDebInfo>:_HAS_ITERATOR_DEBUGGING=0>"
	)

	add_compile_options($<$<PLATFORM_ID:Windows>:/MP${Biogears_BUILD_PARALLEL_LEVEL}>)	# other proj enable multi-processor compilation

	target_compile_options(${name}
		PRIVATE $<$<PLATFORM_ID:Windows>:/std:c17>
		PRIVATE $<$<PLATFORM_ID:Windows>:/bigobj>
		PRIVATE $<$<PLATFORM_ID:Windows>:/MP${Biogears_BUILD_PARALLEL_LEVEL}>	# this proj enable multi-processor compilation
	)

	set_target_properties(${name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

else()
	#target_precompile_headers(${name}
	#	PRIVATE	$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/xx2d.h>
	#)
	#set_property(TARGET ${name} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)	# todo: only for release?
endif()



include_directories(
	imgui/misc/cpp
)

set(SRCS)
file(GLOB SRCS	#GLOB_RECURSE
	imgui/misc/cpp/*.h
	imgui/misc/cpp/*.cpp
)
add_library(imguicpp ${SRCS})

target_link_libraries(${name} imguicpp)





SET(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW_BUILD_DOCS" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW_BUILD_EXAMPLES" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW_BUILD_TESTS" FORCE)
SET(GLFW_INSTALL OFF CACHE BOOL "GLFW_INSTALL" FORCE)
add_subdirectory(glfw)


add_subdirectory(imgui_cmake imgui)


add_subdirectory(pugixml)


SET(BUILD_TESTING OFF CACHE BOOL "BUILD_TESTING" FORCE)
SET(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "USE_MSVC_RUNTIME_LIBRARY_DLL" FORCE)
SET(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "ZSTD_BUILD_PROGRAMS" FORCE)
SET(ZSTD_BUILD_SHARED OFF CACHE BOOL "ZSTD_BUILD_SHARED" FORCE)
SET(ZSTD_BUILD_TESTS OFF CACHE BOOL "ZSTD_BUILD_SHARED" FORCE)
SET(ZSTD_LEGACY_SUPPORT OFF CACHE BOOL "ZSTD_LEGACY_SUPPORT" FORCE)
add_subdirectory(zstd/build/cmake zstd)
include_directories(
	zstd/lib
)



###########################################################################################################################################
###########################################################################################################################################
###########################################################################################################################################
# examples


include_directories(
	examples
)

set(SRCS)
file(GLOB SRCS	#GLOB_RECURSE
	examples/*.h
	examples/*.hpp
	examples/*.inc
	examples/*.c
	examples/*.cpp
)
add_executable(examples ${SRCS})

target_link_libraries(examples ${name} glfw imgui pugixml libzstd_static)


if(MSVC)	# vs2022+

	target_link_options(examples
		PRIVATE $<$<CONFIG:MinSizeRel>:/LTCG>
		PRIVATE $<$<CONFIG:Release>:/LTCG>
		PRIVATE $<$<CONFIG:RelWithDebInfo>:/LTCG>
	)

	target_compile_options(examples
		PRIVATE $<$<PLATFORM_ID:Windows>:/std:c17>
		PRIVATE $<$<PLATFORM_ID:Windows>:/bigobj>
	)

	set_source_files_properties(examples/main.cpp PROPERTIES COMPILE_FLAGS "/Ycmain.h")
	target_compile_options(examples PRIVATE $<$<PLATFORM_ID:Windows>:/Yumain.h>)

	set_target_properties(examples PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

endif()




###########################################################################################################################################
###########################################################################################################################################
###########################################################################################################################################
# tools

# movepath editor

include_directories(
	tools/movepath_editor
)

set(SRCS)
file(GLOB SRCS	#GLOB_RECURSE
	tools/movepath_editor/*.h
	tools/movepath_editor/*.hpp
	tools/movepath_editor/*.inc
	tools/movepath_editor/*.c
	tools/movepath_editor/*.cpp
)
add_executable(tools_movepath_editor ${SRCS})

target_link_libraries(tools_movepath_editor ${name} glfw imgui imguicpp pugixml libzstd_static)


if(MSVC)	# vs2022+

	target_link_options(tools_movepath_editor
		PRIVATE $<$<CONFIG:MinSizeRel>:/LTCG>
		PRIVATE $<$<CONFIG:Release>:/LTCG>
		PRIVATE $<$<CONFIG:RelWithDebInfo>:/LTCG>
	)

	target_compile_options(tools_movepath_editor
		PRIVATE $<$<PLATFORM_ID:Windows>:/std:c17>
		PRIVATE $<$<PLATFORM_ID:Windows>:/bigobj>
	)

	set_source_files_properties(tools/movepath_editor/pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
	target_compile_options(tools_movepath_editor PRIVATE $<$<PLATFORM_ID:Windows>:/Yupch.h>)

	set_target_properties(tools_movepath_editor PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

endif()


# todo: circle line editor?
