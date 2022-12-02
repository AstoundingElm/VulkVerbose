#include "keys.h"
#include "vulkan_types.h"
#include "vulkan_util.h"
#include "shader_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vulkan_instance.h"
#include "platform.h"
#include "surface.h"
#include "vulkan_device.h"
#include "command_buffers.h"
#include "swapchain.h"
#include "image_views.h"
#include "renderpass.h"
#include "framebuffers.h"
#include "sync_objects.h"
//#include "descriptor_sets.h"
#include "pipeline.h"
#include "buffers.h"

#include "renderloop.h"

bool get_framebuffer_size(vulkan_types* context, platform_state* platform_context)
{
        context->framebuffer_context.framebuffer_width = platform_context->win_width;
        context->framebuffer_context.framebuffer_height = platform_context->win_height;
        return true;
}

int main(){
        platform_state plat_context = {0};
        
        PASSERT(platform_startup( &plat_context, "Test Application",100 , 100, 800, 600), "Platform startup failed");
        
        PASSERT(get_framebuffer_size(&vulkan_context, &plat_context), "failed to get framebuffer size\n");
        
        
        PASSERT(create_instance(&vulkan_context),  "Instance creation failed");
        
        PASSERT(create_surface(&vulkan_context, &plat_context), "Surface creation failed");
        PASSERT(create_device(&vulkan_context), "Device creation failed");
        
        PASSERT(create_command_pool(&vulkan_context), "Command pool creation failed");
        
        PASSERT(create_swapchain(&vulkan_context, &plat_context), "Failed to create swapchain");
        
        PASSERT(get_swapchain_image_views(&vulkan_context), "Image views failed");
        
        
        PASSERT(create_render_pass(&vulkan_context), "Failed to create render pass");
        
        
        PASSERT(create_framebuffers(&vulkan_context, &plat_context), "Failed to create framebuffers");
        
        
        PASSERT(create_command_buffer(&vulkan_context, true), "command buffers faield to create");
        
        
        PASSERT(create_sync_objects(&vulkan_context), "failed to create sync object");
        
        //PASSERT(create_descriptor_layouts(&vulkan_context), "Failed to create descriptor layouts");
        //this is not used
        
        PASSERT(create_object_shader(&vulkan_context, false, &vulkan_context.main_shader), "Pipeline failed to create");
        
        
        vulkan_buffer_create(&vulkan_context,    sizeof(global_uniform_object),
                             VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             true,
                             &vulkan_context.main_shader.global_uniform_buffer);
        
        // Allocate global descriptor sets.
        VkDescriptorSetLayout global_layouts[3] = {
                vulkan_context.main_shader.global_descriptor_set_layout,
                vulkan_context.main_shader.global_descriptor_set_layout,
                vulkan_context.main_shader.global_descriptor_set_layout};
        
        VkDescriptorSetAllocateInfo alloc_info = {0};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.pNext = NULL;
        alloc_info.descriptorPool = vulkan_context.main_shader.global_descriptor_pool;
        alloc_info.descriptorSetCount = 3;
        alloc_info.pSetLayouts = global_layouts;
        VK_CHECK(vkAllocateDescriptorSets(vulkan_context.device_context.logical_device, &alloc_info, vulkan_context.main_shader.global_descriptor_sets));
        
        //then create_buffer
        
        PASSERT(create_buffers(&vulkan_context), "Failed to create buffers");
        
        PASSERT(upload_data_ranges(&vulkan_context), "Failed to uplaod data ranges");
        
        
        //PASSERT(vulkan_object_shader_update_global_state(&vulkan_context), "failed to update descriptor/shader");
        
        /*while(test){
                
                platform_pump_messages(&plat_context);
                vulkan_renderer_backend_begin_frame();
                
        }*/
        
        
        
        while (test) {
                platform_pump_messages(&plat_context);
                render_loop();
                
        }
        
}
