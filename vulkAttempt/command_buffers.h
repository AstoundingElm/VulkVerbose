#pragma once

#include "defines.h"

PINLINE bool create_command_pool(vulkan_types * context){
        
        /*VkCommandPoolCreateInfo transient_command_pool_create_info = {0};
        transient_command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        transient_command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        transient_command_pool_create_info.queueFamilyIndex = context->queue_context.graphics_family_index;
        VK_CHECK(vkCreateCommandPool(context->device_context.logical_device, &transient_command_pool_create_info, context->instance_context.allocator, &context->command_pool_context.transientCommandPool));
        */
        
        VkCommandPoolCreateInfo primary_command_pool_create_info = {0};
        primary_command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        primary_command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        primary_command_pool_create_info.queueFamilyIndex = context->queue_context.graphics_family_index;
        VK_CHECK(vkCreateCommandPool(context->device_context.logical_device, &primary_command_pool_create_info, context->instance_context.allocator, &context->command_pool_context.primaryCommandPool));
        
        
        /*VkCommandPoolCreateInfo command_pool_create_info = {0};
        command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        command_pool_create_info.queueFamilyIndex = context->queue_context.graphics_family_index;
        VK_CHECK(vkCreateCommandPool(context->device_context.logical_device, &command_pool_create_info, context->instance_context.allocator, &context->command_pool_context.primaryCommandPool));
        */
        puts("command pool created\n");
        return true;
}


PINLINE bool create_command_buffer(vulkan_types* context,  bool is_primary )
{
        VkCommandBufferAllocateInfo alloc_info = {0};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = context->command_pool_context.primaryCommandPool;
        alloc_info.level = is_primary ?VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        alloc_info.commandBufferCount = 1;
        alloc_info.pNext = 0;
        VK_CHECK(vkAllocateCommandBuffers(
                                          context->device_context.logical_device,
                                          &alloc_info,
                                          &context->graphics_command_buffer.handle));
        puts("command buffer created\n");
        return true;
}



void vulkan_command_buffer_allocate(
                                    vulkan_types* context,
                                    VkCommandPool pool,
                                    b8 is_primary,
                                    vulkan_command_buffer* out_command_buffer) {
        
        memset(out_command_buffer, 0, sizeof(*out_command_buffer));
        
        VkCommandBufferAllocateInfo allocate_info = {0};
        allocate_info.sType  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocate_info.pNext = NULL;
        allocate_info.commandPool = pool;
        allocate_info.level = is_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocate_info.commandBufferCount = 1;
        allocate_info.pNext = 0;
        
        out_command_buffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
        VK_CHECK(vkAllocateCommandBuffers(
                                          context->device_context.logical_device,
                                          &allocate_info,
                                          &out_command_buffer->handle));
        out_command_buffer->state = COMMAND_BUFFER_STATE_READY;
}

void vulkan_command_buffer_free(
                                vulkan_types* context,
                                VkCommandPool pool,
                                vulkan_command_buffer* command_buffer) {
        vkFreeCommandBuffers(
                             context->device_context.logical_device,
                             pool,
                             1,
                             &command_buffer->handle);
        
        command_buffer->handle = 0;
        command_buffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
}

void vulkan_command_buffer_end(vulkan_command_buffer* command_buffer) {
        VK_CHECK(vkEndCommandBuffer(command_buffer->handle));
        command_buffer->state = COMMAND_BUFFER_STATE_RECORDING_ENDED;
}

void vulkan_command_buffer_update_submitted(vulkan_command_buffer* command_buffer) {
        command_buffer->state = COMMAND_BUFFER_STATE_SUBMITTED;
}

void vulkan_command_buffer_reset(vulkan_command_buffer* command_buffer) {
        command_buffer->state = COMMAND_BUFFER_STATE_READY;
}

void vulkan_command_buffer_begin(
                                 vulkan_command_buffer* command_buffer,
                                 b8 is_single_use,
                                 b8 is_renderpass_continue,
                                 b8 is_simultaneous_use) {
        
        VkCommandBufferBeginInfo begin_info = {0};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.pNext = NULL;
        begin_info.flags = 0;
        if (is_single_use) {
                begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        }
        if (is_renderpass_continue) {
                begin_info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        }
        if (is_simultaneous_use) {
                begin_info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        }
        
        VK_CHECK(vkBeginCommandBuffer(command_buffer->handle, &begin_info));
        command_buffer->state = COMMAND_BUFFER_STATE_RECORDING;
}

void vulkan_command_buffer_allocate_and_begin_single_use(
                                                         vulkan_types* context,
                                                         VkCommandPool pool,
                                                         vulkan_command_buffer* out_command_buffer) {
        vulkan_command_buffer_allocate(context, pool, true, out_command_buffer);
        vulkan_command_buffer_begin(out_command_buffer, true, false, false);
}

void vulkan_command_buffer_end_single_use(
                                          vulkan_types* context,
                                          VkCommandPool pool,
                                          vulkan_command_buffer* command_buffer,
                                          VkQueue queue) {
        
        // End the command buffer.
        vulkan_command_buffer_end(command_buffer);
        
        // Submit the queue
        VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer->handle;
        VK_CHECK(vkQueueSubmit(queue, 1, &submit_info, 0));
        
        // Wait for it to finish
        VK_CHECK(vkQueueWaitIdle(queue));
        
        // Free the command buffer.
        vulkan_command_buffer_free(context, pool, command_buffer);
}

