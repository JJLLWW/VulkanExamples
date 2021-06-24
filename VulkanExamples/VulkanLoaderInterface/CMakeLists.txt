cmake_minimum_required(VERSION 3.14)

project(VkloaderInterface
        VERSION 1.0.0
        DESCRIPTION "Boilerplate code to detect and interface with the official Vulkan Loader"
        )

add_library(${PROJECT_NAME} STATIC)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_include_directories(${PROJECT_NAME} 
        PUBLIC
        include
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_sources(${PROJECT_NAME} 
        PRIVATE
        src/vkli.cpp
        src/os-specific.cpp
        src/vkli-helpers.cpp
)

# OS specific code
if(APPLE)
    message(FATAL_ERROR "${PROJECT_NAME}: Vulkan is not supported on MacOS (!)")
elseif(WIN32)
    message(STATUS "${PROJECT_NAME}: Using Windows platform.")
    target_compile_definitions(${PROJECT_NAME} PRIVATE OS_WINDOWS VK_USE_PLATFORM_WIN32_KHR)
elseif(UNIX)
    message(STATUS "${PROJECT_NAME}: Using Linux platform.")
    target_compile_definitions(${PROJECT_NAME} PRIVATE OS_LINUX VK_USE_PLATFORM_XLIB_KHR)
    find_package(X11 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC X11::X11 dl)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Headers GLFW::GLFW)
add_library(VKLInterface::VKLInterface ALIAS ${PROJECT_NAME})