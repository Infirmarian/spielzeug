add_library(tools SHARED)

find_package(Tracy REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp")
target_sources(tools PRIVATE ${SOURCES})


target_compile_options(tools PRIVATE ${COMPILER_FLAGS})
target_link_libraries(tools PUBLIC Tracy::TracyClient)
