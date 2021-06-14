/*
    vkli-internal.hpp: Header for Vulkan Loader Interface internal helper functions and data.

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

#include <vector>
#include <memory>

#define DEFINE_VKAPI_FUNC(fun) PFN_##fun fun


namespace vkli {
    namespace helpers {
        VkInstance GetRawInstance(VkInstanceCreateInfo *create_info = nullptr);
    }

    namespace os {
        void LoadEntrypoint();
    }
}