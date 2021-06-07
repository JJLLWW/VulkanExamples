cmake_minimum_required(VERSION 3.5.0)
project("vulkan_examples" 
        VERSION 1.0.0 
        DESCRIPTION "example code for the vulkan graphics API"
        LANGUAGES C CXX) 

set(CMAKE_CXX_STANDARD 20)

# if CMAKE_BUILD_TYPE is unspecified by the user, it defaults to the value it
# was set to on the last invocation of cmake, as this is cached in CMakeCache.txt.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug Build")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Release Build")
endif()

# OS specific code
if(APPLE)
    message(FATAL_ERROR "Vulkan is not supported on MacOS (!)")
elseif(WIN32)
    message(FATAL_ERROR "Windows support unfinished (!)")
elseif(UNIX)
    message(STATUS "Using Linux platform")
    add_compile_definitions(OS_LINUX)
    set(RUNTIME_LOAD_LIB "dl")
endif()

set(VULKAN_HEADERS "external/Vulkan-Headers")
file(GLOB EXAMPLE1_SRC src/example1/*.cpp)

add_executable("example1" ${EXAMPLE1_SRC})
target_include_directories("example1" PUBLIC ${VULKAN_HEADERS} src)
target_link_libraries("example1" ${RUNTIME_LOAD_LIB})