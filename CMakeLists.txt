cmake_minimum_required(VERSION 3.20)

set(name xx2d)
project(${name})

set(CMAKE_CXX_STANDARD 20)

include_directories(
	xxlib
	src
	./
)
link_directories(
	glfw
)

set(SRCS)
file(GLOB_RECURSE SRCS
    glad/*.h
    glad/*.c
    glfw/*.h
	src/*.cpp
	src/*.h
	src/*.hpp
	src/*.inc
)
add_executable(${name} ${SRCS})

#add_subdirectory(glfw)
#target_link_libraries(${name} glfw)

if(MSVC)
	target_link_libraries(${name} opengl32 glfw3)
	SET_TARGET_PROPERTIES(${name} PROPERTIES COMPILE_FLAGS "/GL")
	SET_TARGET_PROPERTIES(${name} PROPERTIES LINK_FLAGS "/LTCG")
	target_compile_options(${name} 
		PRIVATE $<$<PLATFORM_ID:Windows>:/bigobj>
		PRIVATE $<$<PLATFORM_ID:Windows>:/MP${Biogears_BUILD_PARALLEL_LEVEL}>)
else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
endif()
