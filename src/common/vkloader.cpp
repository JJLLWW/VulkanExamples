/*
    vkloader.cpp: Implementation of Vulkan Loader detection and usage interface. 
*/

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

#define INCLUDED_FROM_LOADER
#include "vkloadedfunc.hpp"

namespace {
    void PlatformSpecificLoadEntrypoint();
    void LoadGlobalLevelFunctions();
    void GetInstance(VkInstance *pinstance, VkInstanceCreateInfo *create_info = nullptr);
    void LoadInstanceLevelFunctions(VkInstance instance);
}

namespace vk {
    bool InitVulkan() {
        try {
            PlatformSpecificLoadEntrypoint(); 
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
            // can't use a smart pointer here, as vkDestroyInstance has not been loaded yet. Ouch.
            VkInstance *instance;
            GetInstance(instance, create_info);
            LoadInstanceLevelFunctions(*instance);
            VkInstance *heap_instance {new VkInstance};
            heap_instance = instance;

            std::shared_ptr<VkInstance> shared_instance(heap_instance, [](VkInstance *inst){
                std::clog << "[INFO] Vulkan instance destroyed" << std::endl;
                vkDestroyInstance(*inst, nullptr);
            });

            std::clog << "[INFO] Vulkan instance created." << std::endl;
            return shared_instance;
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
    void PlatformSpecificLoadEntrypoint() {
            #if defined(OS_LINUX)
                void *dlhandle {dlopen("libvulkan.so", RTLD_LAZY)};
            #elif defined(OS_WINDOWS)
                HMODULE dlhandle {LoadLibrary("vulkan-1.dll")};
            #else
                void *dlhandle {nullptr};
            #endif

            if(dlhandle == nullptr)
                throw std::runtime_error("[ERROR] The vulkan loader (libvulkan.so | vulkan-1.dll) could not be found!"); 

            #if defined(OS_LINUX)
                void *EntrypointAddr {dlsym(dlhandle, "vkGetInstanceProcAddr")};
            #elif defined(OS_WINDOWS)
                FARPROC EntrypointAddr {GetProcAddress(dlhandle, "vkGetInstanceProcAddr")};
            #else
                PFN_vkGetInstanceProcAddr EntrypointAddr {nullptr};
            #endif

            vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(EntrypointAddr);

            if(vkGetInstanceProcAddr == nullptr)
                throw std::runtime_error("[ERROR] Vulkan loader found, but loading vkGetInstanceProcAddr failed.");
    }

    void LoadGlobalLevelFunctions() {
        #define LOAD_GLOBAL_FUNCS
        #include "vkloadedfunc.hpp"
        #undef LOAD_GLOBAL_FUNCS
    }

    void GetInstance(VkInstance *pinstance, VkInstanceCreateInfo *create_info) {
        if(create_info != nullptr) 
            if(vkCreateInstance(create_info, nullptr, pinstance) != VK_SUCCESS)
                throw std::runtime_error("[ERROR] Instance creation failed");
            else return;

        VkApplicationInfo default_app_info {
            VK_STRUCTURE_TYPE_APPLICATION_INFO,
            nullptr,
            nullptr,
            0,
            nullptr,
            0,
            0
        };

        VkInstanceCreateInfo default_create_info {
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            nullptr,
            0,
            nullptr,
            0,
            nullptr,
            0,
            nullptr
        }; 

        if(vkCreateInstance(&default_create_info, nullptr, pinstance) != VK_SUCCESS)
            throw std::runtime_error("[ERROR] Instance creation failed");
    }

    void LoadInstanceLevelFunctions(VkInstance instance) {
        #define LOAD_INSTANCE_FUNCS
        #include "vkloadedfunc.hpp"
        #undef LOAD_INSTANCE_FUNCS
    }
}