#include "vkloader.hpp"

int main() {
    if(vk::InitVulkan())
        vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties");
}



