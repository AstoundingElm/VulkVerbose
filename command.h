#pragma once

#include "defines.h"

PINLINE bool create_command_pool(vulkan_types * context){
        
        VkCommandPoolCreateInfo transient_command_pool_create_info = {0};
        transient_command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        transient_command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        transient_command_pool_create_info.queueFamilyIndex = context->graphics_family_index;
        VK_CHECK(vkCreateCommandPool(context->logical_device, &transient_command_pool_create_info, context->allocator, &context->transientCommandPool));
        
        
        VkCommandPoolCreateInfo primary_command_pool_create_info = {0};
        primary_command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        primary_command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        primary_command_pool_create_info.queueFamilyIndex = context->graphics_family_index;
        VK_CHECK(vkCreateCommandPool(context->logical_device, &primary_command_pool_create_info, context->allocator, &context->primaryCommandPool));
        
        
        VkCommandPoolCreateInfo command_pool_create_info = {0};
        command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        command_pool_create_info.queueFamilyIndex = context->graphics_family_index;
        VK_CHECK(vkCreateCommandPool(context->logical_device, &command_pool_create_info, context->allocator, &context->primaryCommandPool));
        
        
        return true;
};


PINLINE bool create_command_buffer(vulkan_types* context,  bool is_primary )
{
        VkCommandBufferAllocateInfo alloc_info = {0};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = context->primaryCommandPool;
        alloc_info.level = is_primary ?VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        alloc_info.commandBufferCount = 1;
        alloc_info.pNext = 0;
        VK_CHECK(vkAllocateCommandBuffers(
                                          context->logical_device,
                                          &alloc_info,
                                          &context->command_buffer));
        
        return true;
}
