#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#ifdef OS_LINUX
    #include <dlfcn.h>
#endif

#define INCLUDED_FROM_LOADER
#include "vkglobalfunc.hpp"

namespace {
    PFN_vkGetInstanceProcAddr PlatformSpecificLoadEntrypoint();
    void DebugLogLayerExt(std::ostream& log = std::clog);

    #define GET_VK_GLOBAL_FUNC(fun) \
    fun = reinterpret_cast<PFN_##fun>(vkGetInstanceProcAddr(nullptr, #fun)); \
    if(fun == nullptr) { \
        throw std::runtime_error("[ERROR] Loading global-level function " #fun " failed."); \
    } 
}

namespace vk {
    bool InitVulkan() {
        try {
            vkGetInstanceProcAddr = PlatformSpecificLoadEntrypoint(); 
            GET_VK_GLOBAL_FUNC(vkCreateInstance); 
            GET_VK_GLOBAL_FUNC(vkEnumerateInstanceExtensionProperties);
            GET_VK_GLOBAL_FUNC(vkEnumerateInstanceLayerProperties);   

            DebugLogLayerExt();
        }
        catch(std::runtime_error& e) {
            std::clog << e.what() << std::endl;
            return false;
        }

        std::clog << "[INFO] Vulkan initialisation successful" << std::endl;
        return true;
    }
}

namespace {
    PFN_vkGetInstanceProcAddr PlatformSpecificLoadEntrypoint() {
            #ifdef OS_LINUX
                void *dlhandle {dlopen("libvulkan.so", RTLD_LAZY)};
            #else
                void *dlhandle {nullptr};
            #endif

            if(dlhandle == nullptr)
                throw std::runtime_error("[ERROR] The vulkan loader (libvulkan.so | vulkan-1.dll) could not be found!"); 

            #ifdef OS_LINUX
                void *EntrypointAddr {dlsym(dlhandle, "vkGetInstanceProcAddr")};
                PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr {reinterpret_cast<PFN_vkGetInstanceProcAddr>(EntrypointAddr)};
            #else
                PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr {nullptr};
            #endif

            if(vkGetInstanceProcAddr == nullptr)
                throw std::runtime_error("[ERROR] Vulkan loader found, but loading vkGetInstanceProcAddr failed.");

            return vkGetInstanceProcAddr;
    }
    void DebugLogLayerExt(std::ostream& log) {
        uint32_t nproperties;
        vkEnumerateInstanceLayerProperties(&nproperties, nullptr);
        std::vector<VkLayerProperties> Properties {nproperties};
        vkEnumerateInstanceLayerProperties(&nproperties, Properties.data());

        log << nproperties << " Available Layers: " << std::endl; 
        for(const auto& Property : Properties) {
            log << Property.layerName << "> " << Property.description << std::endl;
        }
    }
}