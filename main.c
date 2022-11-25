#include "keys.h"
#include "vulkan_types.h"
#include "vulkan_util.h"
#include "shader_utils.h"
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>  
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>  
#include <sys/time.h>

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
#include "command.h"
#include "framebuffers.h"
#include "sync_objects.h"
#include "descriptor_sets.h"
#include "pipeline.h"



typedef struct {
        int i;
        int e;
        
}testese;


int main(int argc, char ** argv){
        
        testese *tes;
        tes->i = 100;
        tes->e = 88;
        
        platform_startup( &state, "Test Application",100 , 100, 800, 600);
        
        if(!create_instance(&vulkan_context)){
                
                return -1;
        }
        
        if(!create_surface(&vulkan_context, &state)){
                return -1;
        }
        
        if(!create_device(&vulkan_context)){
                return -1;
        }
        if(!create_command_pool(&vulkan_context)){
                return -1;
                abort();
        }
        
        if(!create_swapchain(&vulkan_context)){
                return -1;
                abort();
        }
        
        if(!get_image_views(&vulkan_context))
        {
                return -1;
        }
        if(!create_render_pass(&vulkan_context))
        {
                return -1;
        }
        
        /*
        if(!create_framebuffers(&vulkan_context))
        {
                return -1;
        };
        
        if(!create_command_buffer(&vulkan_context, true))
        {
                return -1;
        }
        
        if(!create_sync_objects(&vulkan_context))
        {
                return -1;
        }
        
        if(!create_pipeline(&vulkan_context, false))
        {
                return -1;
        }*/
        while(test){
                
                platform_pump_messages(&state);
        }
        
}
