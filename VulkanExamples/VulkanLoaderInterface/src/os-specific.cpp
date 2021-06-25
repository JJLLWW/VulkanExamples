/*
    os-specific.cpp: Implementation of Vulkan Loader detection and usage interface.

    - File for any operating system specific code in the implementation.

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

#include <stdexcept>

#if defined(OS_LINUX)
#include <dlfcn.h>
#elif defined(OS_WINDOWS)
#include <windows.h>
#endif

namespace vkli {
    namespace os {
        void LoadEntrypoint() {
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
        };
    }
}