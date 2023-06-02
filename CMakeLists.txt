cmake_minimum_required(VERSION 3.20)

set(name xx2d)
project(${name})

if (MSVC)
else()
	if (${CMAKE_BUILD_TYPE} MATCHES "Rel*")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto -ffat-lto-objects")
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto -ffat-lto-objects")
	endif()
endif()

option(XX2D_ENABLE_IMGUI "Enable imgui support" ON)
if (XX2D_ENABLE_IMGUI)
	add_definitions("-DXX2D_ENABLE_IMGUI")
endif()

option(XX2D_ENABLE_MINIAUDIO "Enable miniaudio support" ON)
if (XX2D_ENABLE_MINIAUDIO)
	add_definitions("-DXX2D_ENABLE_MINIAUDIO")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


include_directories(
	glad
	stb
	glfw/include
	pugixml/src
	imgui
	imgui/backends
	miniaudio
	libvpx_prebuilt/includes
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

	target_link_options(${name} PUBLIC
		$<$<CONFIG:Debug>:/NODEFAULTLIB:libcmtd.lib>
		$<$<CONFIG:MinSizeRel>:/NODEFAULTLIB:libcmt.lib>
		$<$<CONFIG:Release>:/NODEFAULTLIB:libcmt.lib>
		$<$<CONFIG:RelWithDebInfo>:/NODEFAULTLIB:libcmt.lib>
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
	target_precompile_headers(${name}
		PRIVATE	$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/xx2d.h>
	)

	target_compile_options(${name} PRIVATE -Wno-invalid-offsetof)
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



add_definitions(-DMAKE_LIB)	# for lua
include_directories(
	lua
)
set(SRCS)
file(GLOB SRCS	#GLOB_RECURSE
	lua/onelua.c
)
add_library(lua ${SRCS})
target_link_libraries(${name} lua)






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


add_subdirectory(spine-runtimes/spine-cpp)
target_compile_options(spine-cpp PRIVATE
		-Wno-parentheses
		-Wno-implicit-fallthrough
		-Wno-type-limits
		)

###########################################################################################################################################
###########################################################################################################################################
###########################################################################################################################################
# examples


include_directories(
	examples
	spine-runtimes/spine-cpp/spine-cpp/include
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

target_link_libraries(examples spine-cpp)

if(MSVC)	# vs2022+
	target_link_libraries(examples ${CMAKE_CURRENT_SOURCE_DIR}/libvpx_prebuilt/lib/windows/vpxmd.lib)
endif()

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

else()
	target_precompile_headers(examples
		PRIVATE	$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/examples/main.h>
	)
endif()



###########################################################################################################################################
###########################################################################################################################################
###########################################################################################################################################
# tests


include_directories(
	tests
	spine-runtimes/spine-cpp/spine-cpp/include
)

set(SRCS)
file(GLOB SRCS	#GLOB_RECURSE
	tests/*.h
	tests/*.hpp
	tests/*.inc
	tests/*.c
	tests/*.cpp
)
add_executable(tests ${SRCS})

target_link_libraries(tests ${name} glfw imgui pugixml libzstd_static)

target_link_libraries(tests spine-cpp)

if(MSVC)	# vs2022+
	target_link_libraries(tests ${CMAKE_CURRENT_SOURCE_DIR}/libvpx_prebuilt/lib/windows/vpxmd.lib)
endif()

if(MSVC)	# vs2022+

	target_link_options(tests
		PRIVATE $<$<CONFIG:MinSizeRel>:/LTCG>
		PRIVATE $<$<CONFIG:Release>:/LTCG>
		PRIVATE $<$<CONFIG:RelWithDebInfo>:/LTCG>
	)

	target_compile_options(tests
		PRIVATE $<$<PLATFORM_ID:Windows>:/std:c17>
		PRIVATE $<$<PLATFORM_ID:Windows>:/bigobj>
	)

	set_source_files_properties(tests/main.cpp PROPERTIES COMPILE_FLAGS "/Ycmain.h")
	target_compile_options(tests PRIVATE $<$<PLATFORM_ID:Windows>:/Yumain.h>)

	set_target_properties(tests PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
else()
	target_precompile_headers(tests
		PRIVATE	$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/tests/main.h>
	)
endif()




###########################################################################################################################################
###########################################################################################################################################
###########################################################################################################################################
# tests2


include_directories(
	tests2
	spine-runtimes/spine-cpp/spine-cpp/include
)

set(SRCS)
file(GLOB SRCS	#GLOB_RECURSE
	tests2/*.h
	tests2/*.hpp
	tests2/*.inc
	tests2/*.c
	tests2/*.cpp
)
add_executable(tests2 ${SRCS})

target_link_libraries(tests2 ${name} glfw imgui pugixml libzstd_static)

target_link_libraries(tests2 spine-cpp)

if(MSVC)	# vs2022+
	target_link_libraries(tests2 ${CMAKE_CURRENT_SOURCE_DIR}/libvpx_prebuilt/lib/windows/vpxmd.lib)
endif()

if(MSVC)	# vs2022+

	target_link_options(tests2
		PRIVATE $<$<CONFIG:MinSizeRel>:/LTCG>
		PRIVATE $<$<CONFIG:Release>:/LTCG>
		PRIVATE $<$<CONFIG:RelWithDebInfo>:/LTCG>
	)

	target_compile_options(tests2
		PRIVATE $<$<PLATFORM_ID:Windows>:/std:c17>
		PRIVATE $<$<PLATFORM_ID:Windows>:/bigobj>
	)

	set_source_files_properties(tests2/main.cpp PROPERTIES COMPILE_FLAGS "/Ycmain.h")
	target_compile_options(tests2 PRIVATE $<$<PLATFORM_ID:Windows>:/Yumain.h>)

	set_target_properties(tests2 PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
else()
	target_precompile_headers(tests2
		PRIVATE	$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/tests2/main.h>
	)
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

	target_compile_options(tools_movepath_editor
		PRIVATE $<$<PLATFORM_ID:Windows>:/std:c17>
		PRIVATE $<$<PLATFORM_ID:Windows>:/bigobj>
	)

	set_source_files_properties(tools/movepath_editor/pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
	target_compile_options(tools_movepath_editor PRIVATE $<$<PLATFORM_ID:Windows>:/Yupch.h>)

	set_target_properties(tools_movepath_editor PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
else()
	target_precompile_headers(tools_movepath_editor
		PRIVATE	$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/tools/movepath_editor/main.h>
	)
endif()



###########################################################################################################################################
###########################################################################################################################################
###########################################################################################################################################
# tools

# pngs 2 webm ( res viewer )


include_directories(
	tools/pngs2webm
)

set(SRCS)
file(GLOB SRCS	#GLOB_RECURSE
	tools/pngs2webm/*.h
	tools/pngs2webm/*.hpp
	tools/pngs2webm/*.inc
	tools/pngs2webm/*.c
	tools/pngs2webm/*.cpp
)
add_executable(tools_pngs2webm ${SRCS})

target_link_libraries(tools_pngs2webm ${name} glfw imgui imguicpp pugixml libzstd_static)
if(MSVC)	# vs2022+
	target_link_libraries(tools_pngs2webm ${CMAKE_CURRENT_SOURCE_DIR}/libvpx_prebuilt/lib/windows/vpxmd.lib)
endif()

if(MSVC)	# vs2022+

	target_compile_options(tools_pngs2webm
		PRIVATE $<$<PLATFORM_ID:Windows>:/std:c17>
		PRIVATE $<$<PLATFORM_ID:Windows>:/bigobj>
	)

	set_source_files_properties(tools/pngs2webm/pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
	target_compile_options(tools_pngs2webm PRIVATE $<$<PLATFORM_ID:Windows>:/Yupch.h>)

	set_target_properties(tools_pngs2webm PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
else()
	target_precompile_headers(tools_pngs2webm
		PRIVATE	$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/tools/pngs2webm/main.h>
	)
endif()
