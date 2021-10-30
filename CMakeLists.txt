cmake_minimum_required(VERSION 3.19)
project(avakar.intrusive_lfstack)

include(FetchContent)
FetchContent_Declare(avakar.mutest SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/mutest")
FetchContent_Declare(avakar.container_of SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/container_of")
FetchContent_MakeAvailable(avakar.mutest avakar.container_of)

add_library(avakar.intrusive_lfstack INTERFACE
	include/avakar/intrusive/lfstack.h
	)
add_library(avakar::intrusive_lfstack ALIAS avakar.intrusive_lfstack)

target_include_directories(avakar.intrusive_lfstack INTERFACE include)
target_compile_features(avakar.intrusive_lfstack INTERFACE cxx_std_11)

target_link_libraries(avakar.intrusive_lfstack INTERFACE avakar::container_of)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang|GNU")
	target_link_libraries(avakar.intrusive_lfstack INTERFACE -latomic)
endif()

add_mutest(test/test.cpp avakar::intrusive_lfstack)
