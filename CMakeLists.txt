cmake_minimum_required(VERSION 3.20)

set(name xx2d)
project(${name})

set(CMAKE_CXX_STANDARD 20)

include_directories(
	3rd/xxlib
	src
	./
)

set(SRCS)
file(GLOB_RECURSE SRCS
	src/*.cpp
	src/*.h
	src/*.hpp
	src/*.inc
)
add_executable(${name} ${SRCS})

add_subdirectory(3rd/glfw glfw)
target_link_libraries(${name} glfw)

#add_subdirectory(3rd/glad glad)
#target_link_libraries(${name} glad)
include_directories(
	3rd/glad/include
)

if(MSVC)
	set_target_properties(${name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

#	target_link_libraries(${name} opengl32 glfw3)

#	add_definitions(-DOPEN_MP_NUM_THREADS=3)
#	SET_TARGET_PROPERTIES(${name} PROPERTIES COMPILE_FLAGS "/openmp")

	SET_TARGET_PROPERTIES(${name} PROPERTIES COMPILE_FLAGS "/GL")
	SET_TARGET_PROPERTIES(${name} PROPERTIES LINK_FLAGS "/LTCG")
	target_compile_options(${name} 
		PRIVATE $<$<PLATFORM_ID:Windows>:/bigobj>
		PRIVATE $<$<PLATFORM_ID:Windows>:/MP${Biogears_BUILD_PARALLEL_LEVEL}>)
else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
endif()
