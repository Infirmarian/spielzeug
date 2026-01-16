set(COMPILER_FLAGS 
    -stdlib=libc++
    -Wall
    -Wextra
    -Werror
    # todo(rgeil): Dynamically determine this?
    -isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND COMPILER_FLAGS -g -O0)
endif()
