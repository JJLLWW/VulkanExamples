/*
    vkli-helpers.cpp: Implementation of Vulkan Loader detection and usage interface.

    Internal helper functions.
*/

#include "vkli-internal.hpp"

#include <stdexcept>

namespace {
    VkApplicationInfo default_app_info {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        nullptr,
        "Vulkan Example Application",
        VK_MAKE_VERSION(1, 0, 0),
        nullptr,
        0,
        VK_API_VERSION_1_2
    };

    std::vector<const char *> default_layers {
        "VK_LAYER_KHRONOS_validation"
    };

    std::vector<const char *> default_extensions {
        #if defined(VK_USE_PLATFORM_XLIB_KHR)
        VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
        #elif defined(VK_USE_PLATFORM_XCB_KHR)
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
        #elif defined(VK_USE_PLATFORM_WIN32_KHR)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        #endif
        VK_KHR_SURFACE_EXTENSION_NAME
        // VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };

    VkInstanceCreateInfo default_create_info {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0,
        &default_app_info,
        static_cast<uint32_t>(default_layers.size()),
        default_layers.data(),
        static_cast<uint32_t>(default_extensions.size()),
        default_extensions.data()
    };
}

namespace vkli {
    namespace helpers {
        VkInstance GetRawInstance(VkInstanceCreateInfo *create_info) {
        VkInstanceCreateInfo *instance_create_info;
        if(create_info != nullptr) {
            instance_create_info = create_info;
        } else {
            instance_create_info = &default_create_info;
        }

        VkInstance result_instance;
        if(vkCreateInstance(instance_create_info, nullptr, &result_instance) != VK_SUCCESS)
            throw std::runtime_error("[ERROR] Instance creation failed");

        return result_instance;
    }
    }
}

