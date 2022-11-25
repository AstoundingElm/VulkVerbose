#pragma once
#include "defines.h"

PINLINE bool create_surface(vulkan_types* context, platform_state * pla_state) {
        
        puts("Creating vulkan surface...\n");
        
        VkXcbSurfaceCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
        create_info.connection = pla_state->connection;
        create_info.window = pla_state->window;
        
        VK_CHECK(vkCreateXcbSurfaceKHR(
                                       context->instance,
                                       &create_info,
                                       context->allocator,
                                       &context->surface));
        
        puts("Surface successfully created!\n");
        return true;
}
