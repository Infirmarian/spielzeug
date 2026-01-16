add_library(glad STATIC)
target_sources(glad PRIVATE ${CMAKE_SOURCE_DIR}/third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/third_party/glad/include)
target_compile_options(glad PRIVATE)


add_library(renderer_gl SHARED)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE MODULES "*.cppm")
target_sources(renderer_gl PUBLIC FILE_SET CXX_MODULES FILES ${MODULES})
target_sources(renderer_gl PRIVATE ${SOURCES})
target_include_directories(renderer_gl PRIVATE 
    ${CMAKE_SOURCE_DIR}/third_party/glad/include
    ${CMAKE_SOURCE_DIR}/third_party
)
target_compile_options(renderer_gl PRIVATE ${COMPILER_FLAGS})

target_link_libraries(renderer_gl
	PUBLIC
        OpenGL::GL
        PRIVATE
        glm::glm
        glad
)
