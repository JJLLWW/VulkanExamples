#include "vkli/vkli.hpp"

#include <vector>
#include <memory>

int main() {
    if(vkli::InitVulkan()) {
        vkli::DebugLogLayerExt();
        std::shared_ptr<VkInstance> instance {vkli::GetVkInstance()};
    }
}



