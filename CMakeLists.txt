cmake_minimum_required(VERSION 3.14)

project("vulkan_examples" 
        VERSION 1.0.0 
        DESCRIPTION "example code for the vulkan graphics API"
        LANGUAGES C CXX) 

# if CMAKE_BUILD_TYPE is unspecified by the user, it defaults to the value it
# was set to on the last invocation of cmake, as this is cached in CMakeCache.txt.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug Build")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Release Build")
endif()

# order of add_subdirectories is important
add_subdirectory("external") 
add_subdirectory("VulkanExamples")