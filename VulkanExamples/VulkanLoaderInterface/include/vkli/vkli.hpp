/*
    vkli.hpp: A high level interface for detecting and using the Vulkan Loader.

    If InitVulkan() returns true, a working system Vulkan Loader was found, and all of
    the instance creation functions are usable. Creating a context makes all other core
    API functions usable.

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

#pragma once

#include "vkli/vkapi.hpp"

#include <iostream>
#include <memory>

namespace vkli {
    bool InitVulkan();
    std::shared_ptr<VkInstance> GetVkInstance(VkInstanceCreateInfo *create_info = nullptr);
    void DebugLogLayerExt(std::ostream& log = std::clog);
}
