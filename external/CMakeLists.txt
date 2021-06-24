# === VULKAN HEADERS ===
# if find_package(Vulkan REQUIRED) has been called, don't mess up its Vulkan::Headers target.
if(TARGET Vulkan::Headers)
    message(STATUS "Using system vulkan headers.")
else()
    message(STATUS "Using in tree vulkan headers.")
    add_library(VulkanHeaders INTERFACE)
    target_include_directories(VulkanHeaders 
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Headers/include/vulkan>
    )
    # only an ALIAS or IMPORTED target can have a :: in its name.
    add_library(Vulkan::Headers ALIAS VulkanHeaders)
endif()

# === GLFW ===
add_subdirectory(glfw)
# dirty, assume the glfw library target is still called glfw.
add_library(GLFW::GLFW ALIAS glfw)