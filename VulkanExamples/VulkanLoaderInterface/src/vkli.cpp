/*
    vkli.cpp: Implementation of Vulkan Loader detection and usage interface.

    -Provides the implementation of the Vulkan Loader class from vkli.hpp.

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
#include "vkli/vkli.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#define VK_ENTRYPOINT_FUNC(fun) PFN_##fun fun
#define VK_GLOBAL_FUNC(fun) PFN_##fun fun
#define VK_INSTANCE_FUNC(fun) PFN_##fun fun
#include "vkli/vkapi.hpp"

namespace vkli {
    VkLoader::VkLoader() : m_Surface{nullptr}, m_Instance{nullptr} {
        glfwInit();
        os::LoadEntrypoint();
        helpers::LoadGlobalLevelFunctions();
        InitLoaderInfo();
        std::clog << "[INFO] Vulkan Loader initialisation successful" << std::endl;
    }

    VkLoader::~VkLoader() {
        // temporary
        if(m_Device) vkDestroyDevice(m_Device, nullptr);
        if(m_Surface) vkDestroySurfaceKHR(m_Instance, *m_Surface, nullptr);
        if(m_Instance) vkDestroyInstance(m_Instance, nullptr);
        glfwTerminate();
    }

    bool VkLoader::CreateInstance(VkInstanceCreateInfo& create_info) {
         try {
            VkInstance instance {helpers::GetRawInstance(&create_info)};
            helpers::LoadInstanceLevelFunctions(instance);
            m_Instance = instance;
            helpers::GetDevices(m_Instance, m_instinfo);
            } catch(std::runtime_error& e) {
            std::clog << e.what() << std::endl;
            return false;
        }

        std::clog << "[INFO] Vulkan instance created." << std::endl;
        return true;
    }

    bool VkLoader::CreateInstance(std::vector<std::string>& layers,
                                std::vector<std::string>& extensions,
                                VkApplicationInfo& app_info) 
    {   
        for(const auto& lyr : layers) { std::clog << "[INFO] Using layer " << lyr << std::endl; }
        for(const auto& ext : extensions) { std::clog << "[INFO] Using extension " << ext << std::endl; }
        uint32_t nlayers, nexts;
        std::vector<const char *> raw_layer_vect, raw_ext_vect;
        const char* const * layers_c;
        const char* const * extensions_c;

        if(layers.empty()) {
            nlayers = 0;
            layers_c = nullptr;
        } else {
            for(const auto& layer : layers) {
                raw_layer_vect.push_back(layer.c_str());
            }
            nlayers = layers.size();
            layers_c = raw_layer_vect.data();
        }
        
        if(extensions.empty()) {
            nexts = 0;
            extensions_c = nullptr;
        } else {
            for(const auto& extension : extensions) {
                raw_ext_vect.push_back(extension.c_str());
            }
            nexts = extensions.size();
            extensions_c = raw_ext_vect.data();
        }

        VkInstanceCreateInfo create_info {
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            nullptr,
            0,
            &app_info,
            nlayers,
            layers_c,
            nexts,
            extensions_c
        };

        return CreateInstance(create_info);
    }

    bool VkLoader::CreateInstance(std::vector<PriorityList>& layers_pl, 
                                  std::vector<PriorityList>& extensions_pl, 
                                  VkApplicationInfo&         app_info) 
    {
        std::vector<std::string> layers, extensions;

        FillFromPriorityLists(layers, layers_pl, LAYER);
        FillFromPriorityLists(extensions, extensions_pl, EXTENSION);

        return CreateInstance(layers, extensions, app_info);
    }

        void VkLoader::InitLoaderInfo() {
        uint32_t nExt, nLyr;
        if(vkEnumerateInstanceExtensionProperties(nullptr, &nExt, nullptr) != VK_SUCCESS ||
           vkEnumerateInstanceLayerProperties(&nLyr, nullptr) != VK_SUCCESS) {
               throw std::runtime_error("[ERROR] Vulkan Loader intialisation failed!");
        }
        std::vector<VkExtensionProperties> ExtProperties {nExt};
        std::vector<VkLayerProperties> LyrProperties {nLyr};
        if(vkEnumerateInstanceExtensionProperties(nullptr, &nExt, ExtProperties.data()) != VK_SUCCESS ||
           vkEnumerateInstanceLayerProperties(&nLyr, LyrProperties.data()) != VK_SUCCESS) {
            throw std::runtime_error("[ERROR] Vulkan Loader intialisation failed!");
        }

        std::vector<std::string> LyrNames, ExtNames;
        for(const auto& prop : ExtProperties) { ExtNames.push_back(prop.extensionName); }
        for(const auto& prop : LyrProperties) { LyrNames.push_back(prop.layerName); }

        m_ldrinfo = {
            .layers = LyrProperties,
            .lyrnames = LyrNames,
            .extensions = ExtProperties,
            .extnames = ExtNames
        };
    }

    bool VkLoader::CreateDevice(VkDeviceCreateInfo& create_info, VkPhysicalDevice& pdev) {
        if(vkCreateDevice(pdev, &create_info, nullptr, &m_Device) != VK_SUCCESS) {
            return false;
        }
        std::clog << "[INFO] Logical device creation successful" << std::endl;
        return true;
    }

    bool VkLoader::CreateDevice(std::vector<std::string>& extensions) {
        // check whether given extensions are supported on any physical devices:

        std::vector<int> capable_device_indeces, capable_qf_indices;
        std::string failed_extension;
        for(int i = 0; i < m_instinfo.n_dev; i++) {
            bool all_supported = true;
            for(const auto& ext : extensions) {
                bool supported = false;
                for(const auto& sup_ext : m_instinfo.dev_exts[i]) {
                    if(ext == sup_ext.extensionName) {
                        supported = true;
                        break;
                    }
                }
                if(!supported) {
                    all_supported = false;
                    failed_extension = ext;
                    break;
                }
            }
            if(all_supported) {
                capable_device_indeces.push_back(i);
            }
        }
         
        if(capable_device_indeces.empty()) {
            std::clog << "[ERROR] No physical device supports the device extension " << failed_extension << std::endl;
            return false;
        }

        // check queue families. 
        for(int i : capable_device_indeces) {
            for(int index = 0; index < m_instinfo.dev_queue[i].size(); index++) {
                if(m_instinfo.dev_queue[i][index].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    capable_qf_indices.push_back(index);
                }
            }
        }

        // create the logical device.
        std::vector<const char *> c_extensions;
        for(const auto& ext : extensions) {
            c_extensions.push_back(ext.c_str());
        } 

        std::vector<float> q_priorities{1.0f};  

        // !!!!! FOR NOW JUST USE THE FIRST CAPABLE DEVICE, ONLY USE ONE QUEUE !!!!!
        VkDeviceQueueCreateInfo queue_info {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            nullptr,
            0,
            static_cast<uint32_t>(capable_qf_indices[0]), // index of q family 
            static_cast<uint32_t>(q_priorities.size()),   // number of qs to create in q family
            q_priorities.data()
        };

        VkDeviceCreateInfo create_info {
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            nullptr,
            0,
            1, // queue families
            &queue_info, // queue families
            0, // layers (deprecated)
            nullptr, // layers (deprecated)
            static_cast<uint32_t>(c_extensions.size()),
            c_extensions.data(),
            nullptr // features
        };
    
        // !!!!! FOR NOW JUST USE THE FIRST CAPABLE DEVICE !!!!!
        m_PhysDevice = m_instinfo.devices[capable_device_indeces[0]];
        return CreateDevice(create_info, m_PhysDevice);
    }

    void VkLoader::FillFromPriorityLists(std::vector<std::string>& output, 
                            const std::vector<PriorityList>& PLists,
                            LyrOrExt                   type) {
        
        for(const auto& PList : PLists) {
            for(const auto& elem : PList) {
                if(type == LAYER) {
                    if(std::find(m_ldrinfo.lyrnames.begin(), m_ldrinfo.lyrnames.end(), elem) != m_ldrinfo.lyrnames.end()) {
                        output.push_back(elem);
                        break;
                    }
                }
                else if(type == EXTENSION) {
                    if(std::find(m_ldrinfo.extnames.begin(), m_ldrinfo.extnames.end(), elem) != m_ldrinfo.extnames.end()) {
                    output.push_back(elem);
                    break;
                    }
                }
            }
        }
    }

    bool VkLoader::CreateSurface() {
        // TEMPORARY FIX
        VkSurfaceKHR *surface = new VkSurfaceKHR;
        m_Window = glfwCreateWindow(1000, 1000, "window", nullptr, nullptr);
        if(glfwCreateWindowSurface(m_Instance, m_Window, nullptr, surface) != VK_SUCCESS)
            return false; 
        m_Surface = surface;
        if(!helpers::GetSwapchainInfo(m_PhysDevice, *m_Surface, m_swapinfo) || helpers::LoadSwapchainDFPs(m_dfps)) 
            return false;

        int width, height;
        glfwGetWindowSize(m_Window, &width, &height);
        VkSwapchainCreateInfoKHR create_info {
            VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            nullptr,
            0,
            *m_Surface,
            1, // minimum number of images
            m_swapinfo.sformats[0].format,
            m_swapinfo.sformats[0].colorSpace,
            {static_cast<uint32_t>(width), static_cast<uint32_t>(height)},
            1, // always 1 if not stereoscopic 3D
            VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM, // all image usage flags
            VK_SHARING_MODE_EXCLUSIVE,
            0,   // unnecessary if not shared
            nullptr, // unnecessary if not shared
            VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_PRESENT_MODE_MAILBOX_KHR, // present mode flags
            VK_TRUE,
            VK_NULL_HANDLE // old swapchain
        };
        VkSwapchainKHR swapchain;
        if(m_dfps.vkCreateSwapchainKHR(m_Device, &create_info, nullptr, m_Swapchain) != VK_SUCCESS)
            return false;
        return true;
    }
}