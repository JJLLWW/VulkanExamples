#include "vkloader.hpp"

#include <vector>
#include <memory>

int main() {
    if(vk::InitVulkan()) {
        vk::DebugLogLayerExt();
        std::shared_ptr<VkInstance> instance {vk::GetVkInstance()};
    }
}



