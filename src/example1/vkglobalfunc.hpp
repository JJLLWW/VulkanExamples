#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#ifdef INCLUDED_FROM_LOADER
#define VK_GLOBAL_FUNC(fun) PFN_##fun fun
#else
#define VK_GLOBAL_FUNC(fun) extern PFN_##fun fun
#endif

// Entrypoint
VK_GLOBAL_FUNC(vkGetInstanceProcAddr);

// Other global level functions
VK_GLOBAL_FUNC(vkCreateInstance);
VK_GLOBAL_FUNC(vkEnumerateInstanceExtensionProperties);
VK_GLOBAL_FUNC(vkEnumerateInstanceLayerProperties);
