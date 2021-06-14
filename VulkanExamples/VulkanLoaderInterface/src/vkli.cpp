/*
    vkli.cpp: Implementation of Vulkan Loader detection and usage interface.

    Provides the implementation of any exposed functions/classes in vkli.hpp.
*/

#include "vkli-internal.hpp"
#define INCLUDED_FROM_LOADER
#include "vkli/vkapi.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#if defined(OS_LINUX)
#include <dlfcn.h>
#elif defined(OS_WINDOWS)
// including just the relevant header (libloaderapi.h) in isolation doesn't work.
#include <windows.h>
#endif

namespace {
    void LoadGlobalLevelFunctions();
    void LoadInstanceLevelFunctions(VkInstance instance);
    void PlatformSpecificCreateSurface(std::shared_ptr<VkInstance> instance);
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
        #define LOAD_GLOBAL_FUNCS
        #include "vkli/vkapi.hpp"
        #undef LOAD_GLOBAL_FUNCS
    }

    

    void LoadInstanceLevelFunctions(VkInstance instance) {
        #define LOAD_INSTANCE_FUNCS
        #include "vkli/vkapi.hpp"
        #undef LOAD_INSTANCE_FUNCS
    }

    void PlatformSpecificCreateSurface(std::shared_ptr<VkInstance> instance) {
    #if defined(VK_USE_PLATFORM_XLIB_KHR)
    // vkCreateXlibSurfaceKHR(instance.get());
    #endif
    }
}