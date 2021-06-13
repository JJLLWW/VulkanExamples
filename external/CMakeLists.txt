if(TARGET Vulkan::Headers)
    message(STATUS "Using system vulkan headers.")
else()
    message(STATUS "Using in tree vulkan headers.")
    # can't call it Vulkan::Headers or get this error:
    # "The target name "Vulkan::Headers" is reserved or not valid for certain CMake features"
    add_library(VulkanHeaders INTERFACE)
    target_include_directories(VulkanHeaders 
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Headers/include/vulkan>
    )
    # "only an ALIAS or IMPORTED target can have a :: in its name"
    add_library(Vulkan::Headers ALIAS VulkanHeaders)
endif()

