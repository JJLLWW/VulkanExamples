/*
    vkli-internal.hpp: Header for Vulkan Loader Interface internal helper functions and data.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.
    
    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "vkli/vkli.hpp"
#include "GLFW/glfw3.h"

#include <vector>
#include <memory>
#include <iostream>
#include <unordered_map>

namespace vkli {
    namespace os {
        void LoadEntrypoint();
    }

    namespace helpers {
        void LoadGlobalLevelFunctions();
        void LoadInstanceLevelFunctions(VkInstance instance);
        VkInstance GetRawInstance(VkInstanceCreateInfo *create_info);
        void GetDevices(VkInstance& inst, InstanceInfo& info);
        bool GetSwapchainInfo(VkPhysicalDevice& dev, VkSurfaceKHR& surface,SwapchainInfo& info);
        bool LoadSwapchainDFPs(DeviceFPs& dfps);
    }
}