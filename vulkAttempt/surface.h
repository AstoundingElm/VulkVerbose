#pragma once
#include "defines.h"
#include "platform_types.h"

PINLINE bool create_surface(vulkan_types* context, platform_state* platform_context) {
        
        puts("Creating vulkan surface...\n");
        
        VkXcbSurfaceCreateInfoKHR create_info = {0};
        
        create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        create_info.pNext = NULL;
        create_info.connection = platform_context->connection;
        create_info.window = platform_context->window;
        
        VK_CHECK(vkCreateXcbSurfaceKHR(
                                       context->instance_context.instance,
                                       &create_info,
                                       context->instance_context.allocator,
                                       &context->surface_context.surface));
        
        puts("Surface successfully created!\n");
        return true;
}
