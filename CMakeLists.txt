cmake_minimum_required(VERSION 3.28)

project(spielzeug)
set(CMAKE_CXX_STANDARD 23)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party")


find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)


add_executable(engine)
target_sources(engine PRIVATE main.cpp)

target_compile_options(engine PRIVATE -stdlib=libc++)
target_compile_options(engine PRIVATE -Wall -Wextra -Werror)
target_compile_options(engine PRIVATE 
    -isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk
)
target_link_directories(engine PRIVATE /opt/homebrew/opt/llvm/lib/c++)
target_link_options(engine PRIVATE -stdlib=libc++ -fuse-ld=lld)

target_link_libraries(engine PRIVATE SDL3::SDL3 glm::glm platform)

add_subdirectory(platform)