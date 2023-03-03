cmake_minimum_required(VERSION 3.8)

project(imgui)

include_directories(
	../glad
	../imgui
	../imgui/backends
	../glfw/include
)

set(IMGUI_SRCS)
file(GLOB IMGUI_SRCS	#GLOB_RECURSE
	../imgui/*.h
	../imgui/*.cpp
	../imgui/backends/imgui_impl_glfw.h
	../imgui/backends/imgui_impl_glfw.cpp
	../imgui/backends/imgui_impl_opengl3.h
	../imgui/backends/imgui_impl_opengl3.cpp
	../imgui/backends/imgui_impl_opengl3_loader.h
)

add_library(imgui STATIC
    ${IMGUI_SRCS}
)
