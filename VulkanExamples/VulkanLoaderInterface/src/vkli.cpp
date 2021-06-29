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

    bool VkLoader::CreateDevice(VkDeviceCreateInfo& create_info) {
        // if(vkCreateDevice())
        return false;
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

        // check queue families
        for(int i : capable_device_indeces) {
            for(int index = 0; index < m_instinfo.dev_queue[i].size(); index++) {
                if(m_instinfo.dev_queue[i][index].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    capable_qf_indices.push_back(index);
                }
            }
        }

        // create the logical device
        std::vector<const char *> c_extensions;
        for(const auto& ext : extensions) {
            c_extensions.push_back(ext.c_str());
        }   

        VkDeviceQueueCreateInfo queue_info {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            nullptr,
            0,
            static_cast<uint32_t>(capable_qf_indices[0]),
            100,    // queueCount  
            nullptr // queue priorities
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
        // VkDevice handle;
        // vkCreateDevice(m_instinfo.devices[0], &test_info, nullptr, &m_Device);
        return true;
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
        return true;
    }
}