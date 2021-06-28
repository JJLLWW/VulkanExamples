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
#include "GLFW/glfw3.h"

#include <vector>
#include <memory>
// temporary
#include <unistd.h>

int main() {    
    vkli::VkLoader test_loader;
    std::vector<std::string> layers {"VK_LAYER_KHRONOS_validation"};
    std::vector<std::string> extensions {VK_KHR_SURFACE_EXTENSION_NAME, 
    #if defined(VK_USE_PLATFORM_XLIB_KHR) 
        "VK_KHR_xlib_surface"
    #endif
    };
    test_loader.CreateInstance(layers, extensions);

    for(const auto& layer : test_loader.m_ldrinfo.layers) {
        std::cerr << layer.layerName << std::endl; 
    }

    std::cout << test_loader.CreateSurface() << std::endl;
    // temporary
    sleep(1);
}



