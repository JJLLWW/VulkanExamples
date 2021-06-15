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
    }
}

