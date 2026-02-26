cmake_minimum_required(VERSION 3.28)

project(spielzeug)
set(CMAKE_CXX_STANDARD 23)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party")
include(cmake/compile_flags.cmake)

find_package(LLVM REQUIRED)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(engine_exe)
target_sources(engine_exe PRIVATE main.cpp)
target_include_directories(engine_exe PRIVATE /opt/homebrew/opt/llvm/include/c++/v1)

target_compile_options(engine_exe PRIVATE ${COMPILER_FLAGS})

target_link_directories(engine_exe PRIVATE /opt/homebrew/opt/llvm/lib/c++)
target_link_options(engine_exe PRIVATE -stdlib=libc++ -fuse-ld=lld)
target_link_libraries(engine_exe PRIVATE glm::glm platform renderer_gl tools engine)

add_subdirectory(platform)
add_subdirectory(renderer/gl)
add_subdirectory(tools)
add_subdirectory(engine)
