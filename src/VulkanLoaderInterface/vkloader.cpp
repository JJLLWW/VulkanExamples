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
    VkApplicationInfo default_app_info {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        nullptr,
        "Vulkan Example Application",
        VK_MAKE_VERSION(1, 0, 0),
        nullptr,
        0,
        VK_API_VERSION_1_2
    };

    std::vector<const char *> default_layers {
        "VK_LAYER_KHRONOS_validation"
    };

    std::vector<const char *> default_extensions {
        #if defined(VK_USE_PLATFORM_XLIB_KHR)
        VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
        #elif defined(VK_USE_PLATFORM_XCB_KHR)
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
        #elif defined(VK_USE_PLATFORM_WIN32_KHR)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        #endif
        VK_KHR_SURFACE_EXTENSION_NAME
        // VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };

    VkInstanceCreateInfo default_create_info {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0,
        &default_app_info,
        static_cast<uint32_t>(default_layers.size()),
        default_layers.data(),
        static_cast<uint32_t>(default_extensions.size()),
        default_extensions.data()
    };

    void PlatformSpecificLoadEntrypoint();
    void LoadGlobalLevelFunctions();
    VkInstance GetRawInstance(VkInstanceCreateInfo *create_info = nullptr);
    void LoadInstanceLevelFunctions(VkInstance instance);
    void PlatformSpecificCreateSurface(std::shared_ptr<VkInstance> instance);
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
            std::shared_ptr<VkInstance> instance(new VkInstance {GetRawInstance(create_info)}, [](VkInstance *inst){
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

    VkInstance GetRawInstance(VkInstanceCreateInfo *create_info) {
        VkInstanceCreateInfo *instance_create_info;
        if(create_info != nullptr) {
            instance_create_info = create_info;
        } else {
            instance_create_info = &default_create_info;
        }

        VkInstance result_instance;
        if(vkCreateInstance(instance_create_info, nullptr, &result_instance) != VK_SUCCESS)
            throw std::runtime_error("[ERROR] Instance creation failed");

        return result_instance;
    }

    void LoadInstanceLevelFunctions(VkInstance instance) {
        #define LOAD_INSTANCE_FUNCS
        #include "vkloadedfunc.hpp"
        #undef LOAD_INSTANCE_FUNCS
    }

    void PlatformSpecificCreateSurface(std::shared_ptr<VkInstance> instance) {
    #if defined(VK_USE_PLATFORM_XLIB_KHR)
    // vkCreateXlibSurfaceKHR(instance.get());
    #endif
    }
}