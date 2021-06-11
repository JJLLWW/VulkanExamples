/*
    vkloader.hpp: A high level interface for detecting and using the Vulkan Loader.

    If InitVulkan() returns true, a working system Vulkan Loader was found, and all of
    the instance creation functions are usable. Creating a context makes all other core
    API functions usable.
*/

#pragma once

#include "vkloadedfunc.hpp"

#include <iostream>
#include <memory>

namespace vk {
    bool InitVulkan();
    extern const VkInstanceCreateInfo default_instance_info, debug_instance_info;
    std::shared_ptr<VkInstance> GetVkInstance(VkInstanceCreateInfo *create_info = nullptr);
    void DebugLogLayerExt(std::ostream& log = std::clog);
}
