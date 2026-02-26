add_library(engine SHARED)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE MODULES "*.cppm")
target_sources(engine PUBLIC FILE_SET CXX_MODULES FILES ${MODULES})
target_sources(engine PRIVATE ${SOURCES})


target_compile_options(engine PRIVATE ${COMPILER_FLAGS})
target_link_libraries(engine PRIVATE glm::glm platform)
