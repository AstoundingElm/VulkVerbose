#include "keys.h"
#include "vulkan_types.h"
#include "vulkan_util.h"
#include "shader_utils.h"

#if _POSIX_C_SOURCE >= 199309L
#include <time.h>  // nanosleep
#else
#include <unistd.h>  // usleep
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vulkan_instance.h"
#include "platform.h"
#include "surface.h"
#include "vulkan_device.h"
#include "command.h"
#include "swapchain.h"
#include "image_views.h"
#include "renderpass.h"
/*
#include "command.h"
#include "framebuffers.h"
#include "sync_objects.h"
#include "descriptor_sets.h"
#include "pipeline.h"
#include "buffers.h"
#include "data_range.h"
*/
int main(){
        platform_state plat_context = {0};
        
        PASSERT(platform_startup( &plat_context, "Test Application",100 , 100, 800, 600), "Platform startup failed");
        
        PASSERT(create_instance(&vulkan_context),  "Instance creation failed");
        
        PASSERT(create_surface(&vulkan_context, &plat_context), "Surface creation failed");
        PASSERT(create_device(&vulkan_context), "Device creation failed");
        
        PASSERT(create_command_pool(&vulkan_context), "Command pool creation failed");
        
        PASSERT(create_swapchain(&vulkan_context, &plat_context), "Failed to create swapchain");
        
        PASSERT(get_swapchain_image_views(&vulkan_context), "Image views failed");
        
        
        PASSERT(create_render_pass(&vulkan_context), "Failed to create render pass");
        
        /*
        if(!create_framebuffers(&vulkan_context))
        {
                return -1;
        }
        
        if(!create_command_buffer(&vulkan_context, true))
        {
                return -1;
        }
        
        if(!create_sync_objects(&vulkan_context))
        {
                return -1;
        }
        
        if(!create_descriptor_layouts(&vulkan_context))
        {
                return -1;
        }
        
        if(!create_pipeline(&vulkan_context, false))
        {
                return -1;
        }
        
        if(!create_buffers(&vulkan_context))
        {
                return -1;
        }
        
        if(!init(vulkan_context))
        {
                return -1;
        }
        if(!vulkan_object_shader_update_global_state(&vulkan_context, &main_shader))
        {
                return -1;
                
        }
        */
        while(test){
                
                platform_pump_messages(&plat_context);
        }
        
}
