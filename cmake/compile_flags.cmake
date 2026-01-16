set(COMPILER_FLAGS 
    -Wall
    -Wextra
    -Werror
    -Weverything
    -Wpedantic

    -Wno-c++98-compat
    -Wno-c++98-compat-pedantic
    -Wno-switch-enum

    # todo(rgeil): Dynamically determine this?
    -isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND COMPILER_FLAGS -g -O0)
endif()
