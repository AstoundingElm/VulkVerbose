#pragma once

#include "defines.h"
#include "maths.h"
typedef struct
{
        VkSwapchainKHR swapchain;
        VkInstance instance;
        
        VkAllocationCallbacks* allocator;
        VkDebugUtilsMessengerEXT debug_messenger;
        
        VkSurfaceKHR surface;
        VkSurfaceCapabilitiesKHR surface_caps;
        VkPresentModeKHR present_mode;
        VkFormat present_format;
        VkSurfaceFormatKHR surfaceFormat;
        VkSurfaceFormatKHR swap_format;
        
        u32 image_count;
        VkImage swapchain_images[8];
        VkImageView swapchain_image_views[8];
        VkFormat depth_format;
        
        VkDevice logical_device;
        u32 physical_device_index;
        VkPhysicalDevice gpu;
        
        VkPhysicalDeviceProperties physical_device_properties;
        
        u32 transfer_family_index;
        u32 compute_family_index;
        u32 graphics_family_index;
        u32 present_family_index;
        u32 graphics_queue_index;
        
        VkExtensionProperties device_extensions[500];
        u32 device_ext_count;
        
        VkQueue graphics_queue;
        
        VkCommandPool transientCommandPool;
        VkCommandPool primaryCommandPool;
        
        /*VkCmdBeginDebugUtilsLabelEXT CmdBeginDebugUtilsLabelEXT;
        vkCmdEndDebugUtilsLabelEXT CmdEndDebugUtilsLabelEXT;
        vkCmdInsertDebugUtilsLabelEXT CmdInsertDebugUtilsLabelEXT;
        vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT;
        VkDebugUtilsLabelEXT label*/
        u32 framebuffer_width;
        u32 framebuffer_height;
        
        VkRenderPass renderpass;
        
        
        u32 attachment_count;
        VkImageView attachments;
        
        VkFramebuffer framebuffer;
        VkCommandBuffer command_buffer;
        
        VkFence fence;
        VkSemaphore aquireSemaphore;
        VkSemaphore submitSemaphore;
        
        VkDescriptorSetLayout set_layout;
        
        VkPipelineLayout pipe_layout;
        VkPipeline pipeline;
        
}vulkan_types;

static vulkan_types vulkan_context = {0};
typedef struct vertex_3d {
        vec3 position;
} vertex_3d;
/*
#define SET_OBJECT_NAME(obj,objName,objType) vk_set_object_name( (uint64_t)(obj), (objName), (objType) )

static void vk_set_object_name( uint64_t obj, const char *objName, VkDebugReportObjectTypeEXT objType, vulkan_types*context )
{
	if ( vkDebugMarkerSetObjectNameEXT && obj )
	{
		VkDebugMarkerObjectNameInfoEXT info;
		info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
		info.pNext = NULL;
		info.objectType = objType;
		info.object = obj;
		info.pObjectName = objName;
		vkDebugMarkerSetObjectNameEXT( context->logical_device, &info );
	}
}

VkDebugUtilsObjectNameInfoEXT cmd_buf_name = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .pNext = NULL,
        .objectType = VK_OBJECT_TYPE_COMMAND_BUFFER,
        .objectHandle = (uint64_t)cmd_buf,
        .pObjectName = "CubeDrawCommandBuf",
};

SetDebugUtilsObjectNameEXT(device, &cmd_buf_name);
// Begin a label section indicating we're in a draw.
label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
label.pNext = NULL;
label.pLabelName = "Inside Draw";
label.color = {0.4f, 0.3f, 0.2f, 0.1f};
CmdBeginDebugUtilsLabelEXT(cmd_buf, &label);
SetDebugUtilsObjectNameEXT(device, &cmd_buf_name);
// Begin a label section indicating we're in a draw.
label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
label.pNext = NULL;
label.pLabelName = "Inside Draw";
label.color = {0.4f, 0.3f, 0.2f, 0.1f};
CmdBeginDebugUtilsLabelEXT(cmd_buf, &label);
// Do other stuff
// Insert a single label
label.pLabelName = "Temp Label";
label.color = {1.0f, 1.0f, 1.0f, 1.0f};
CmdInsertDebugUtilsLabelEXT(cmd_buf, &label);
// End the label (this rolls back to any label before "Inside Draw")
// since that is the last one that was created with CmdBeginDebugUtilsLabelEXT
CmdEndDebugUtilsLabelEXT(cmd_buf);

label.pLabelName = "Temp Label";
label.color = {1.0f, 1.0f, 1.0f, 1.0f};
CmdInsertDebugUtilsLabelEXT(cmd_buf, &label);
// End the label (this rolls back to any label before "Inside Draw")
// since that is the last one that was created with CmdBeginDebugUtilsLabelEXT
CmdEndDebugUtilsLabelEXT(cmd_buf)

VkDebugMarkerObjectTagInfoEXT tagInfo = {};
tagInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_TAG_INFO_EXT;
tagInfo.objectType = VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT;
tagInfo.object = (uint64_t)someBuffer;
tagInfo.tagName = 0x01;
// Size of the arbitrary data structure 
tagInfo.tagSize = sizeof(customTag);
// Pointer to the arbitrary data
tagInfo.pTag = &customTag;
pfnDebugMarkerSetObjectTag(device, &tagInfo);



VkDebugMarkerMarkerInfoEXT markerInfo = {};
markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
memcpy(markerInfo.color, &color[0], sizeof(float) * 4);
markerInfo.pMarkerName = "Set primary viewport";
pfnCmdDebugMarkerInsert(cmdbuffer, &markerInfo);
*/

