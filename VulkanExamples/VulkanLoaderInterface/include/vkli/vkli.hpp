/*
    vkli.hpp: A high level interface for detecting and using the Vulkan Loader.

    If VkLoader's constructor does not throw, a working Vulkan Loader was found, and all of
    the instance creation functions are usable. Creating a context makes all other core
    API functions usable, as well as (EVENTUALLY) a set of supported extensions.

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
#include "GLFW/glfw3.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace vkli {
    inline VkApplicationInfo default_app_info {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        nullptr,
        "Vulkan Example Application",
        VK_MAKE_VERSION(1, 0, 0),
        nullptr,
        0,
        VK_API_VERSION_1_2
    };

    // PriorityList[0] is tested, if it is valid it is used, otherwise the next element is tested, and so on.
    typedef std::vector<std::string> PriorityList;
    typedef std::vector<VkExtensionProperties> Extensions;
    typedef std::vector<VkQueueFamilyProperties> Queues;

    enum LyrOrExt {LAYER, EXTENSION};

    struct LoaderInfo {
        std::vector<VkLayerProperties> layers;
        std::vector<std::string> lyrnames;
        std::vector<VkExtensionProperties> extensions;
        std::vector<std::string> extnames;

    };

    struct InstanceInfo {
        uint32_t n_dev;
        std::vector<VkPhysicalDevice> devices;
        std::vector<Extensions> dev_exts;
        std::vector<Queues> dev_queue;
        std::vector<VkPhysicalDeviceProperties> dev_props;
        std::vector<VkPhysicalDeviceFeatures> dev_feat;
        void resize() { devices.resize(n_dev); dev_exts.resize(n_dev); dev_props.resize(n_dev); 
                        dev_feat.resize(n_dev); dev_queue.resize(n_dev); }
    };

    struct SwapchainInfo {
        VkSurfaceCapabilitiesKHR scapabilities;
        std::vector<VkSurfaceFormatKHR> sformats;
        std::vector<VkPresentModeKHR> prmodes;
    };

    struct DeviceFPs {
        VkDevice dev;
        PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
    };

    class VkLoader {
        public:
            // this constructor will throw a std::runtime_error if a working Vulkan Loader cannot be found.
            VkLoader();
            ~VkLoader();
            bool CreateInstance(VkInstanceCreateInfo& create_info);
            bool CreateInstance(std::vector<std::string>& layers,
                                std::vector<std::string>& extensions,
                                VkApplicationInfo& app_info = default_app_info);
            bool CreateInstance(std::vector<PriorityList>& layers, 
                                std::vector<PriorityList>& extensions, 
                                VkApplicationInfo& app_info = default_app_info);
            bool CreateDevice(VkDeviceCreateInfo& create_info, VkPhysicalDevice& pdev);
            bool CreateDevice(std::vector<std::string>& extensions);
            bool CreateSurface();
        public:
            LoaderInfo m_ldrinfo;
            InstanceInfo m_instinfo;
            SwapchainInfo m_swapinfo;
        private:
            void InitLoaderInfo();
            void FillFromPriorityLists(std::vector<std::string>& output, 
                                       const std::vector<PriorityList>& PLists,
                                       LyrOrExt                   type);
        private:
            VkInstance m_Instance;
            VkDevice m_Device; // for now just use 1 device
            VkPhysicalDevice m_PhysDevice;
            GLFWwindow *m_Window;
            VkSurfaceKHR *m_Surface; // temporary
            DeviceFPs m_dfps;
    };
}
