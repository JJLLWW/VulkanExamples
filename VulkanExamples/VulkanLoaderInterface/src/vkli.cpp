/*
    vkli.cpp: Implementation of Vulkan Loader detection and usage interface.

    -Provides the implementation of any exposed functions/classes in vkli.hpp.

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

#include "vkli-internal.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#define VK_ENTRYPOINT_FUNC(fun) PFN_##fun fun
#define VK_GLOBAL_FUNC(fun) PFN_##fun fun
#define VK_INSTANCE_FUNC(fun) PFN_##fun fun
#include "vkli/vkapi.hpp"


namespace {
    void LoadGlobalLevelFunctions();
    void LoadInstanceLevelFunctions(VkInstance instance);
}

namespace vkli {
    bool InitVulkan() {
        try {
            os::LoadEntrypoint();
            LoadGlobalLevelFunctions();
            std::clog << "[INFO] Vulkan initialisation successful" << std::endl;
            return true;
        }
        catch(std::runtime_error& e) {
            std::clog << e.what() << std::endl;
            return false;
        }
    }

    std::shared_ptr<VkInstance> GetVkInstance(VkInstanceCreateInfo *create_info) {
        try {
            std::shared_ptr<VkInstance> instance(
                new VkInstance {helpers::GetRawInstance(create_info)}, 
                [](VkInstance *inst){
                    std::clog << "[INFO] Vulkan instance destroyed" << std::endl;
                    if(vkDestroyInstance != nullptr)
                        vkDestroyInstance(*inst, nullptr);
                    delete inst; 
            });
            LoadInstanceLevelFunctions(*instance);

            std::clog << "[INFO] Vulkan instance created." << std::endl;
            return instance;
        }
        catch(std::runtime_error& e) {
            std::clog << e.what() << std::endl;
            return nullptr;
        }
    }

    void DebugLogLayerExt(std::ostream& log) {
        uint32_t nlayers, nextensions;
        vkEnumerateInstanceLayerProperties(&nlayers, nullptr);
        vkEnumerateInstanceExtensionProperties(nullptr, &nextensions, nullptr);
        std::vector<VkLayerProperties> Properties {nlayers};
        std::vector<VkExtensionProperties> Extensions {nextensions};
        vkEnumerateInstanceLayerProperties(&nlayers, Properties.data());
        vkEnumerateInstanceExtensionProperties(nullptr, &nextensions, Extensions.data());

        log << nlayers << " Available Layers: " << std::endl;
        for(const auto& Property : Properties) {
            log << Property.layerName << "> " << Property.description << std::endl;
        }
        log << nextensions << " Available Extensions: " << std::endl;
        for(const auto& Extension : Extensions) {
            log << Extension.extensionName << "> " << Extension.specVersion << std::endl;
        }
    }
}

namespace {
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
}