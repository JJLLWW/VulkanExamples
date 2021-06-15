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
    VkLoader::VkLoader() {
        os::LoadEntrypoint();
        helpers::LoadGlobalLevelFunctions();
        std::clog << "[INFO] Vulkan initialisation successful" << std::endl;
    }

     std::optional<std::vector<std::string>> VkLoader::ListSupportedLayers() const {
        uint32_t nLyr;
        vkEnumerateInstanceLayerProperties(&nLyr, nullptr);
        std::vector<VkLayerProperties> LyrProperties {nLyr};
        std::vector<std::string> LyrNames;

        if(vkEnumerateInstanceLayerProperties(&nLyr, LyrProperties.data()) != VK_SUCCESS)
            return {};

        for(const auto& Property : LyrProperties) {
            LyrNames.push_back(Property.layerName);
        }

        // Return Value Optimization
        return LyrNames;
    }

    std::optional<std::vector<std::string>> VkLoader::ListSupportedExt() const {
        uint32_t nExt;
        vkEnumerateInstanceExtensionProperties(nullptr, &nExt, nullptr);
        std::vector<VkExtensionProperties> ExtProperties {nExt};
        std::vector<std::string> ExtNames;
        
        if(vkEnumerateInstanceExtensionProperties(nullptr, &nExt, ExtProperties.data()) != VK_SUCCESS)
            return {};

        for(const auto& Property : ExtProperties) {
            ExtNames.push_back(Property.extensionName);
        }

        // Return Value Optimization
        return ExtNames;
    }

    bool VkLoader::CreateInstance(VkInstanceCreateInfo& create_info) {
         try {
            std::shared_ptr<VkInstance> instance(new VkInstance {helpers::GetRawInstance(&create_info)}, helpers::InstanceDeleter);
            helpers::LoadInstanceLevelFunctions(*instance);
            m_Instance = instance;
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

    // if none of the listed etensions are valid, create a default vkInstance.
    bool VkLoader::CreateInstance(std::vector<PriorityList>& layers_pl, 
                                  std::vector<PriorityList>& extensions_pl, 
                                  VkApplicationInfo&         app_info) 
    {
        std::vector<std::string> layers, extensions;

        FillFromPriorityLists(layers, layers_pl, app_info, LAYER);
        FillFromPriorityLists(extensions, extensions_pl, app_info, EXTENSION);

        return CreateInstance(layers, extensions, app_info);
    }

    void VkLoader::FillFromPriorityLists(std::vector<std::string>& output, 
                            const std::vector<PriorityList>& PLists,
                            VkApplicationInfo&         app_info,
                            LyrOrExt                   type) {
        for(const auto& PList : PLists) {
            for(const auto& elem : PList) {
                std::vector<std::string> velem {elem};
                std::vector<std::string> empty;
                bool supported;
                if(type == LAYER) {
                    supported = CreateInstance(velem, empty, app_info);
                } else if(type == EXTENSION) {
                    supported = CreateInstance(empty, velem, app_info);
                }
                if(supported) {
                    output.push_back(elem);
                    break;
                }       
            }
        }
    }

    std::optional<std::vector<std::string>> VkLoader::ListPhysicalDevs() const {

    }
}