/*
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

#include <vector>
#include <memory>

int main() {
    vkli::VkLoader test_loader;
    std::vector<vkli::PriorityList> layers { { "VK_LAYER_KHRONOS_validation"} };
    std::vector<vkli::PriorityList> extensions {{"VK_KHR_surface"}};
    test_loader.CreateInstance(layers, extensions);
    auto opt_exts {test_loader.ListSupportedExt()};
    auto opt_lyrs {test_loader.ListSupportedLayers()};

    if(opt_exts.has_value()) {
        for(const auto& ext : opt_exts.value()) {
            std::clog << std::string(ext) << std::endl;
        }   
    }

    if(opt_lyrs.has_value()) {
        for(const auto& lyr : opt_lyrs.value()) {
            std::clog << std::string(lyr) << std::endl;
        }
    }

    auto dev_properties {test_loader.ListPhysicalDevices()};
    if(!opt_exts.has_value())
        std::clog << "NO PHYSICAL DEVICES!" << std::endl;

    for(const auto& dev : dev_properties.value()) {
        std::clog << dev.deviceName << std::endl;
    }
}



