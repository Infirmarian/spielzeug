add_library(platform SHARED)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

file(GLOB_RECURSE SOURCES "*.cpp")
target_sources(platform PUBLIC FILE_SET CXX_MODULES FILES sdl.cppm)
target_sources(platform PRIVATE ${SOURCES})

target_compile_options(platform PRIVATE 
    -isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk
)
target_link_libraries(platform PRIVATE SDL3::SDL3)
