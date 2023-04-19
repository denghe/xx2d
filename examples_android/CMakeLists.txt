cmake_minimum_required(VERSION 3.20)

project(ImGuiExample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRCS)
file(GLOB SRCS	#GLOB_RECURSE
	${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/../xxlib/*.h
	#...
)
add_library(${CMAKE_PROJECT_NAME} SHARED ${SRCS})

set(CMAKE_SHARED_LINKER_FLAGS
  "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate"
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
  ${ANDROID_NDK}/sources/android/native_app_glue
  ${CMAKE_CURRENT_SOURCE_DIR}/../xxlib
  #...
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
  android
  EGL
  GLESv3
  log
)
