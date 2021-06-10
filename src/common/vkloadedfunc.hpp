/* 
    vkloadedfunc.hpp: Provides function pointer declarations for the core Vulkan API functions, these function
    pointers are defined and implemented if a working Vulkan Loaoder is detected.
*/

#define VK_NO_PROTOTYPES
#include "vulkan/vulkan.h"

#if defined(INCLUDED_FROM_LOADER)
#define VK_ENTRYPOINT_FUNC(fun) PFN_##fun fun
#define VK_GLOBAL_FUNC(fun) PFN_##fun fun
#define VK_INSTANCE_FUNC(fun) PFN_##fun fun
#else
#define VK_ENTRYPOINT_FUNC(fun) extern PFN_##fun fun
#define VK_GLOBAL_FUNC(fun) extern PFN_##fun fun
#define VK_INSTANCE_FUNC(fun) extern PFN_##fun fun
#endif

#ifdef LOAD_GLOBAL_FUNCS
#undef VK_GLOBAL_FUNC
#define VK_GLOBAL_FUNC(fun) \
fun = reinterpret_cast<PFN_##fun>(::vkGetInstanceProcAddr(nullptr, #fun)); \
    if(fun == nullptr) { \
        throw std::runtime_error("[ERROR] Loading global-level function " #fun " failed."); \
    } 
#endif

#ifdef LOAD_INSTANCE_FUNCS
#undef VK_INSTANCE_FUNC
#define VK_INSTANCE_FUNC(fun) \
fun = reinterpret_cast<PFN_##fun>(::vkGetInstanceProcAddr(instance, #fun)); \
    if(fun == nullptr) { \
        throw std::runtime_error("[ERROR] Loading instance-level function " #fun " failed."); \
    } 
#endif


// Entrypoint to the loader.
VK_ENTRYPOINT_FUNC(vkGetInstanceProcAddr);

// Global level functions
VK_GLOBAL_FUNC(vkCreateInstance);
VK_GLOBAL_FUNC(vkEnumerateInstanceExtensionProperties);
VK_GLOBAL_FUNC(vkEnumerateInstanceLayerProperties);

// Core instance level functions
VK_INSTANCE_FUNC(vkDestroyInstance);
VK_INSTANCE_FUNC(vkEnumeratePhysicalDevices);
VK_INSTANCE_FUNC(vkGetDeviceProcAddr);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceProperties);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceQueueFamilyProperties);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceMemoryProperties);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceFeatures);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceFormatProperties);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceImageFormatProperties);
VK_INSTANCE_FUNC(vkCreateDevice);
VK_INSTANCE_FUNC(vkDestroyDevice);
VK_INSTANCE_FUNC(vkEnumerateInstanceVersion);
VK_INSTANCE_FUNC(vkEnumerateDeviceLayerProperties);
VK_INSTANCE_FUNC(vkEnumerateDeviceExtensionProperties);
VK_INSTANCE_FUNC(vkGetDeviceQueue);
VK_INSTANCE_FUNC(vkQueueSubmit);
VK_INSTANCE_FUNC(vkQueueWaitIdle);
VK_INSTANCE_FUNC(vkDeviceWaitIdle);
VK_INSTANCE_FUNC(vkAllocateMemory);
VK_INSTANCE_FUNC(vkFreeMemory);
VK_INSTANCE_FUNC(vkMapMemory);
VK_INSTANCE_FUNC(vkUnmapMemory);
VK_INSTANCE_FUNC(vkFlushMappedMemoryRanges);
VK_INSTANCE_FUNC(vkInvalidateMappedMemoryRanges);
VK_INSTANCE_FUNC(vkGetDeviceMemoryCommitment);
VK_INSTANCE_FUNC(vkGetBufferMemoryRequirements);
VK_INSTANCE_FUNC(vkBindBufferMemory);
VK_INSTANCE_FUNC(vkGetImageMemoryRequirements);
VK_INSTANCE_FUNC(vkBindImageMemory);
VK_INSTANCE_FUNC(vkGetImageSparseMemoryRequirements);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceSparseImageFormatProperties);
VK_INSTANCE_FUNC(vkQueueBindSparse);
VK_INSTANCE_FUNC(vkCreateFence);
VK_INSTANCE_FUNC(vkDestroyFence);
VK_INSTANCE_FUNC(vkResetFences);
VK_INSTANCE_FUNC(vkGetFenceStatus);
VK_INSTANCE_FUNC(vkWaitForFences);
VK_INSTANCE_FUNC(vkCreateSemaphore);
VK_INSTANCE_FUNC(vkDestroySemaphore);
VK_INSTANCE_FUNC(vkCreateEvent);
VK_INSTANCE_FUNC(vkDestroyEvent);
VK_INSTANCE_FUNC(vkGetEventStatus);
VK_INSTANCE_FUNC(vkSetEvent);
VK_INSTANCE_FUNC(vkResetEvent);
VK_INSTANCE_FUNC(vkCreateQueryPool);
VK_INSTANCE_FUNC(vkDestroyQueryPool);
VK_INSTANCE_FUNC(vkGetQueryPoolResults);
VK_INSTANCE_FUNC(vkResetQueryPool);
VK_INSTANCE_FUNC(vkCreateBuffer);
VK_INSTANCE_FUNC(vkDestroyBuffer);
VK_INSTANCE_FUNC(vkCreateBufferView);
VK_INSTANCE_FUNC(vkDestroyBufferView);
VK_INSTANCE_FUNC(vkCreateImage);
VK_INSTANCE_FUNC(vkDestroyImage);
VK_INSTANCE_FUNC(vkGetImageSubresourceLayout);
VK_INSTANCE_FUNC(vkCreateImageView);
VK_INSTANCE_FUNC(vkDestroyImageView);
VK_INSTANCE_FUNC(vkCreateShaderModule);
VK_INSTANCE_FUNC(vkDestroyShaderModule);
VK_INSTANCE_FUNC(vkCreatePipelineCache);
VK_INSTANCE_FUNC(vkDestroyPipelineCache);
VK_INSTANCE_FUNC(vkGetPipelineCacheData);
VK_INSTANCE_FUNC(vkMergePipelineCaches);
VK_INSTANCE_FUNC(vkCreateGraphicsPipelines);
VK_INSTANCE_FUNC(vkCreateComputePipelines);
VK_INSTANCE_FUNC(vkDestroyPipeline);
VK_INSTANCE_FUNC(vkCreatePipelineLayout);
VK_INSTANCE_FUNC(vkDestroyPipelineLayout);
VK_INSTANCE_FUNC(vkCreateSampler);
VK_INSTANCE_FUNC(vkDestroySampler);
VK_INSTANCE_FUNC(vkCreateDescriptorSetLayout);
VK_INSTANCE_FUNC(vkDestroyDescriptorSetLayout);
VK_INSTANCE_FUNC(vkCreateDescriptorPool);
VK_INSTANCE_FUNC(vkDestroyDescriptorPool);
VK_INSTANCE_FUNC(vkResetDescriptorPool);
VK_INSTANCE_FUNC(vkAllocateDescriptorSets);
VK_INSTANCE_FUNC(vkFreeDescriptorSets);
VK_INSTANCE_FUNC(vkUpdateDescriptorSets);
VK_INSTANCE_FUNC(vkCreateFramebuffer);
VK_INSTANCE_FUNC(vkDestroyFramebuffer);
VK_INSTANCE_FUNC(vkCreateRenderPass);
VK_INSTANCE_FUNC(vkDestroyRenderPass);
VK_INSTANCE_FUNC(vkGetRenderAreaGranularity);
VK_INSTANCE_FUNC(vkCreateCommandPool);
VK_INSTANCE_FUNC(vkDestroyCommandPool);
VK_INSTANCE_FUNC(vkResetCommandPool);
VK_INSTANCE_FUNC(vkAllocateCommandBuffers);
VK_INSTANCE_FUNC(vkFreeCommandBuffers);
VK_INSTANCE_FUNC(vkBeginCommandBuffer);
VK_INSTANCE_FUNC(vkEndCommandBuffer);
VK_INSTANCE_FUNC(vkResetCommandBuffer);
VK_INSTANCE_FUNC(vkCmdBindPipeline);
VK_INSTANCE_FUNC(vkCmdSetViewport);
VK_INSTANCE_FUNC(vkCmdSetScissor);
VK_INSTANCE_FUNC(vkCmdSetLineWidth);
VK_INSTANCE_FUNC(vkCmdSetDepthBias);
VK_INSTANCE_FUNC(vkCmdSetBlendConstants);
VK_INSTANCE_FUNC(vkCmdSetDepthBounds);
VK_INSTANCE_FUNC(vkCmdSetStencilCompareMask);
VK_INSTANCE_FUNC(vkCmdSetStencilWriteMask);
VK_INSTANCE_FUNC(vkCmdSetStencilReference);
VK_INSTANCE_FUNC(vkCmdBindDescriptorSets);
VK_INSTANCE_FUNC(vkCmdBindIndexBuffer);
VK_INSTANCE_FUNC(vkCmdBindVertexBuffers);
VK_INSTANCE_FUNC(vkCmdDraw);
VK_INSTANCE_FUNC(vkCmdDrawIndexed);
VK_INSTANCE_FUNC(vkCmdDrawIndirect);
VK_INSTANCE_FUNC(vkCmdDrawIndexedIndirect);
VK_INSTANCE_FUNC(vkCmdDispatch);
VK_INSTANCE_FUNC(vkCmdDispatchIndirect);
VK_INSTANCE_FUNC(vkCmdCopyBuffer);
VK_INSTANCE_FUNC(vkCmdCopyImage);
VK_INSTANCE_FUNC(vkCmdBlitImage);
VK_INSTANCE_FUNC(vkCmdCopyBufferToImage);
VK_INSTANCE_FUNC(vkCmdCopyImageToBuffer);
VK_INSTANCE_FUNC(vkCmdUpdateBuffer);
VK_INSTANCE_FUNC(vkCmdFillBuffer);
VK_INSTANCE_FUNC(vkCmdClearColorImage);
VK_INSTANCE_FUNC(vkCmdClearDepthStencilImage);
VK_INSTANCE_FUNC(vkCmdClearAttachments);
VK_INSTANCE_FUNC(vkCmdResolveImage);
VK_INSTANCE_FUNC(vkCmdSetEvent);
VK_INSTANCE_FUNC(vkCmdResetEvent);
VK_INSTANCE_FUNC(vkCmdWaitEvents);
VK_INSTANCE_FUNC(vkCmdPipelineBarrier);
VK_INSTANCE_FUNC(vkCmdBeginQuery);
VK_INSTANCE_FUNC(vkCmdEndQuery);
VK_INSTANCE_FUNC(vkCmdResetQueryPool);
VK_INSTANCE_FUNC(vkCmdWriteTimestamp);
VK_INSTANCE_FUNC(vkCmdCopyQueryPoolResults);
VK_INSTANCE_FUNC(vkCmdPushConstants);
VK_INSTANCE_FUNC(vkCmdBeginRenderPass);
VK_INSTANCE_FUNC(vkCmdNextSubpass);
VK_INSTANCE_FUNC(vkCmdEndRenderPass);
VK_INSTANCE_FUNC(vkCmdExecuteCommands);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceFeatures2);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceProperties2);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceFormatProperties2);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceImageFormatProperties2);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceQueueFamilyProperties2);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceMemoryProperties2);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceSparseImageFormatProperties2);
VK_INSTANCE_FUNC(vkTrimCommandPool);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceExternalBufferProperties);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceExternalSemaphoreProperties);
VK_INSTANCE_FUNC(vkGetPhysicalDeviceExternalFenceProperties);
VK_INSTANCE_FUNC(vkEnumeratePhysicalDeviceGroups);
VK_INSTANCE_FUNC(vkGetDeviceGroupPeerMemoryFeatures);
VK_INSTANCE_FUNC(vkBindBufferMemory2);
VK_INSTANCE_FUNC(vkBindImageMemory2);
VK_INSTANCE_FUNC(vkCmdSetDeviceMask);
VK_INSTANCE_FUNC(vkCmdDispatchBase);
VK_INSTANCE_FUNC(vkCreateDescriptorUpdateTemplate);
VK_INSTANCE_FUNC(vkDestroyDescriptorUpdateTemplate);
VK_INSTANCE_FUNC(vkUpdateDescriptorSetWithTemplate);
VK_INSTANCE_FUNC(vkGetBufferMemoryRequirements2);
VK_INSTANCE_FUNC(vkGetImageMemoryRequirements2);
VK_INSTANCE_FUNC(vkGetImageSparseMemoryRequirements2);
VK_INSTANCE_FUNC(vkCreateSamplerYcbcrConversion);
VK_INSTANCE_FUNC(vkDestroySamplerYcbcrConversion);
VK_INSTANCE_FUNC(vkGetDeviceQueue2);
VK_INSTANCE_FUNC(vkGetDescriptorSetLayoutSupport);
VK_INSTANCE_FUNC(vkCreateRenderPass2);
VK_INSTANCE_FUNC(vkCmdBeginRenderPass2);
VK_INSTANCE_FUNC(vkCmdNextSubpass2);
VK_INSTANCE_FUNC(vkCmdEndRenderPass2);
VK_INSTANCE_FUNC(vkGetSemaphoreCounterValue);
VK_INSTANCE_FUNC(vkWaitSemaphores);
VK_INSTANCE_FUNC(vkSignalSemaphore);
VK_INSTANCE_FUNC(vkCmdDrawIndirectCount);
VK_INSTANCE_FUNC(vkCmdDrawIndexedIndirectCount);
VK_INSTANCE_FUNC(vkGetBufferOpaqueCaptureAddress);
VK_INSTANCE_FUNC(vkGetBufferDeviceAddress);
VK_INSTANCE_FUNC(vkGetDeviceMemoryOpaqueCaptureAddress);
