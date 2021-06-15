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

#include "vkli/vkapi.hpp"

#include <vector>
#include <memory>
#include <iostream>

typedef void (*VkInstanceDeleter)(VkInstance*);

namespace vkli {
    namespace os {
        void LoadEntrypoint();
    }

    // helper functions
    namespace helpers {
        void LoadGlobalLevelFunctions();
        void LoadInstanceLevelFunctions(VkInstance instance);
        VkInstance GetRawInstance(VkInstanceCreateInfo *create_info);
    }

    // helper data
    namespace helpers {
        // TEMPORARY HACK WARNING: the sType field is changed, so that code passing a reference can have 
        // a null case.
        inline VkInstanceCreateInfo NO_CREATE_INFO = {VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, 0, nullptr, 0, nullptr, 0};
        inline VkInstanceDeleter InstanceDeleter = [](VkInstance *inst) {
            std::clog << "[INFO] Vulkan instance destroyed" << std::endl;
            if(vkDestroyInstance != nullptr)
                vkDestroyInstance(*inst, nullptr);
            delete inst; 
        };
    }
}