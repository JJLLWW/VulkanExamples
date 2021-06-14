/*
    vkli-internal.hpp: Header for Vulkan Loader Interface internal helper functions and data.
*/

#pragma once

#include "vkli/vkapi.hpp"

#include <vector>
#include <memory>

namespace vkli {
    namespace helpers {
        VkInstance GetRawInstance(VkInstanceCreateInfo *create_info = nullptr);
    }

    namespace os {
        void LoadEntrypoint();
    }
}