#pragma once

#include "defines.h"
#include "maths.h"
#include "platform_types.h"
#include <stdio.h>

typedef struct vulkan_image {
        VkImage handle;
        VkDeviceMemory memory;
        VkImageView view;
        u32 width;
        u32 height;
} vulkan_image;

typedef struct vulkan_buffer {
        u64 total_size;
        VkBuffer handle;
        VkBufferUsageFlagBits usage;
        b8 is_locked;
        VkDeviceMemory memory;
        i32 memory_index;
        u32 memory_property_flags;
} vulkan_buffer;

typedef enum vulkan_command_buffer_state {
        COMMAND_BUFFER_STATE_READY,
        COMMAND_BUFFER_STATE_RECORDING,
        COMMAND_BUFFER_STATE_IN_RENDER_PASS,
        COMMAND_BUFFER_STATE_RECORDING_ENDED,
        COMMAND_BUFFER_STATE_SUBMITTED,
        COMMAND_BUFFER_STATE_NOT_ALLOCATED
} vulkan_command_buffer_state;

typedef struct vulkan_command_buffer {
        VkCommandBuffer handle;
        
        // Command buffer state.
        vulkan_command_buffer_state state;
} vulkan_command_buffer;

typedef struct vulkan_shader_stage {
        VkShaderModuleCreateInfo create_info;
        VkShaderModule handle;
        VkPipelineShaderStageCreateInfo shader_stage_create_info;
} vulkan_shader_stage;

typedef struct global_uniform_object {
        mat4 projection;   // 64 bytes
        mat4 view;         // 64 bytes
        mat4 m_reserved0;  // 64 bytes, reserved for future use
        mat4 m_reserved1;  // 64 bytes, reserved for future use
} global_uniform_object;

#define OBJECT_SHADER_STAGE_COUNT 2
typedef struct vulkan_object_shader {
        // vertex, fragment
        vulkan_shader_stage stages[OBJECT_SHADER_STAGE_COUNT];
        
        VkDescriptorPool global_descriptor_pool;
        VkDescriptorSetLayout global_descriptor_set_layout;
        
        // One descriptor set per frame - max 3 for triple-buffering.
        VkDescriptorSet global_descriptor_sets[3];
        
        // Global uniform object.
        global_uniform_object global_ubo;
        
        // Global uniform buffer.
        vulkan_buffer global_uniform_buffer;
        
        //vulkan_pipeline pipeline;
        
} vulkan_object_shader;

static vulkan_object_shader main_shader;

typedef struct 
{
        VkInstance instance;
        VkAllocationCallbacks* allocator;
        VkDebugUtilsMessengerEXT debug_messenger;
        
        
}instance_types;

typedef struct
{
        
        VkSurfaceCapabilitiesKHR surface_caps;
        VkSurfaceKHR surface;
        
        
}surface_types;

typedef struct
{
        VkDevice logical_device;
        VkPhysicalDeviceProperties physical_device_properties;
        
        u32 physical_device_index;
        VkPhysicalDevice gpu;
        VkExtensionProperties device_extensions[500];
        u32 device_ext_count;
        
        
        
}device_types;

typedef struct
{
        u32 graphics_queue_index;
        VkQueue graphics_queue;
        u32 transfer_family_index;
        u32 compute_family_index;
        u32 present_family_index;
        u32 graphics_family_index;
        
}queue_types;

typedef struct
{
        VkCommandPool transientCommandPool;
        VkCommandPool primaryCommandPool;
        
}command_pool_types;

typedef struct
{
        PFN_vkCmdBeginDebugUtilsLabelEXT CmdBeginDebugUtilsLabelEXT;
        PFN_vkCmdEndDebugUtilsLabelEXT CmdEndDebugUtilsLabelEXT;
        PFN_vkCmdInsertDebugUtilsLabelEXT CmdInsertDebugUtilsLabelEXT;
        PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT;
        VkDebugUtilsLabelEXT label;
        
}vulkan_debug_types;

typedef struct
{
        VkSwapchainKHR swapchain;
        VkPresentModeKHR swap_present_mode;
        VkSurfaceFormatKHR swap_format;
        VkExtent2D swapchain_extent;
        u32 swapchain_image_count;
        VkImage *swapchain_images;
        VkImageView *swapchain_image_views;
}swapchain_types;

typedef struct
{
        vulkan_image depth_attachment;
        VkFormat depth_format;
        
}depth_types;

typedef struct
{
        VkRenderPass renderpass;
}renderpass_types;

typedef struct
{
        vulkan_command_buffer* graphics_command_buffer;
        queue_types queue_context;
        surface_types surface_context;
        instance_types instance_context;
        device_types device_context;
        command_pool_types command_pool_context;
        swapchain_types swapchain_context;
        renderpass_types renderpass_context;
        depth_types depth_context;
        
        //u32 image_index;
        
        
        /*
        vulkan_buffer object_vertex_buffer;
        vulkan_buffer object_index_buffer;
        u64 geometry_vertex_offset;
        u64 geometry_index_offset;
        
        
        u32 current_frame;
        
        u32 framebuffer_width;
        u32 framebuffer_height;
        
        
        
        u32 attachment_count;
        VkImageView attachments[2];
        VkVertexInputAttributeDescription input_attribute_descriptions[1];
        VkFramebuffer framebuffer;
        
        VkFence fence;
        VkSemaphore aquireSemaphore;
        VkSemaphore submitSemaphore;
        
        VkDescriptorSetLayout set_layout;
        
        VkPipelineLayout pipe_layout;
        VkPipeline pipeline;
        VkDescriptorPool global_descriptor_pool;
        VkDescriptorSetLayout  global_descriptor_set_layout;
        */
}vulkan_types;

static vulkan_types vulkan_context;
typedef struct vertex_3d {
        vec3 position;
} vertex_3d;

i32 find_memory_index(u32 type_filter, u32 property_flags, vulkan_types * context) {
        VkPhysicalDeviceMemoryProperties memory_properties;
        vkGetPhysicalDeviceMemoryProperties(context->device_context.gpu, &memory_properties);
        
        for (u32 i = 0; i < memory_properties.memoryTypeCount; ++i) {
                // Check each memory type to see if its bit is set to 1.
                if (type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags) {
                        return i;
                }
        }
        
        printf("Unable to find suitable memory type!");
        return -1;
}


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

