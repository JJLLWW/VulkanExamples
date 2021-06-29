/*
    vkli-helpers.cpp: Implementation of Vulkan Loader detection and usage interface.

    Internal helper functions.

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

#include "vkli/vkli.hpp"
#include "vkli-internal.hpp"

#include <stdexcept>
#include <cstring> // memcpy

namespace vkli {
    namespace helpers {
        void LoadGlobalLevelFunctions() {
            #define LOAD_GLOBAL_FUNC(fun) \
            fun = reinterpret_cast<PFN_##fun>(::vkGetInstanceProcAddr(nullptr, #fun)); \
            if(fun == nullptr) { \
                throw std::runtime_error("[ERROR] Loading instance-level function " #fun " failed."); \
            } 

            #define VK_GLOBAL_FUNC LOAD_GLOBAL_FUNC
            #include "vkli/vkapi.hpp"
            #undef VK_GLOBAL_FUNC
        }

        void LoadInstanceLevelFunctions(VkInstance instance) {
            #define LOAD_INSTANCE_FUNC(fun) \
            fun = reinterpret_cast<PFN_##fun>(::vkGetInstanceProcAddr(instance, #fun)); \
            if(fun == nullptr) { \
                throw std::runtime_error("[ERROR] Loading instance-level function " #fun " failed."); \
            } 

            #define VK_INSTANCE_FUNC LOAD_INSTANCE_FUNC
            #include "vkli/vkapi.hpp"
            #undef VK_INSTANCE_FUNC
        }

        VkInstance GetRawInstance(VkInstanceCreateInfo *create_info) {
            VkInstance result_instance;
            if(vkCreateInstance(create_info, nullptr, &result_instance) != VK_SUCCESS)
                throw std::runtime_error("[ERROR] Instance creation failed");

            return result_instance;
        }

        void GetDevices(VkInstance& inst, InstanceInfo& info) {
            uint32_t n_dev;
            if(vkEnumeratePhysicalDevices(inst, &n_dev, nullptr) != VK_SUCCESS) 
                throw std::runtime_error("[ERROR] Detecting physical devices failed");

            info.n_dev = n_dev;
            info.resize();

            if(vkEnumeratePhysicalDevices(inst, &n_dev, info.devices.data()) != VK_SUCCESS)
                throw std::runtime_error("[ERROR] Detecting physical devices failed");

            for(int i = 0; i < n_dev; i++) {
                vkGetPhysicalDeviceProperties(info.devices[i], &info.dev_props[i]);
                vkGetPhysicalDeviceFeatures(info.devices[i], &info.dev_feat[i]);

                // queues
                uint32_t n_queue;
                vkGetPhysicalDeviceQueueFamilyProperties(info.devices[i], &n_queue, nullptr);
                info.dev_queue[i].resize(n_queue);
                vkGetPhysicalDeviceQueueFamilyProperties(info.devices[i], &n_queue, info.dev_queue[i].data());

                // extensions
                uint32_t n_ext;
                if(vkEnumerateDeviceExtensionProperties(info.devices[i], nullptr, &n_ext, nullptr) != VK_SUCCESS)
                    throw std::runtime_error("[ERROR] Detecting physical device extensions failed");
                info.dev_exts[i].resize(n_ext);
                if(vkEnumerateDeviceExtensionProperties(info.devices[i], nullptr, &n_ext, info.dev_exts[i].data()) != VK_SUCCESS)
                    throw std::runtime_error("[ERROR] Detecting physical device extensions failed");
            }
        }
    }
}

