/*
    vkli.hpp: A high level interface for detecting and using the Vulkan Loader.

    If InitVulkan() returns true, a working system Vulkan Loader was found, and all of
    the instance creation functions are usable. Creating a context makes all other core
    API functions usable.
*/

#pragma once

#include "vkli/vkapi.hpp"

#include <iostream>
#include <memory>

namespace vkli {
    bool InitVulkan();
    std::shared_ptr<VkInstance> GetVkInstance(VkInstanceCreateInfo *create_info = nullptr);
    void DebugLogLayerExt(std::ostream& log = std::clog);
}
