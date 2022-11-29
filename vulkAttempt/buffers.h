#pragma once

#include "vulkan_types.h"
#include <stdio.h>

void vulkan_buffer_bind(vulkan_types* context, vulkan_buffer* buffer, u64 offset) {
        VK_CHECK(vkBindBufferMemory(context->device_context.logical_device, buffer->handle, buffer->memory, offset));
}

b8 vulkan_buffer_create(
                        vulkan_types* context,
                        u64 size,
                        VkBufferUsageFlagBits usage,
                        u32 memory_property_flags,
                        b8 bind_on_create,
                        vulkan_buffer* out_buffer) {
        memset(out_buffer,0,  sizeof(vulkan_buffer));
        out_buffer->total_size = size;
        out_buffer->usage = usage;
        out_buffer->memory_property_flags = memory_property_flags;
        
        VkBufferCreateInfo buffer_info = {0}; buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.pNext = NULL;
        buffer_info.size = size;
        buffer_info.usage = usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // NOTE: Only used in one queue.
        
        VK_CHECK(vkCreateBuffer(context->device_context.logical_device, &buffer_info, context->instance_context.allocator, &out_buffer->handle));
        
        // Gather memory requirements.
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(context->device_context.logical_device, out_buffer->handle, &requirements);
        out_buffer->memory_index = find_memory_index(requirements.memoryTypeBits, out_buffer->memory_property_flags, context);
        if (out_buffer->memory_index == -1) {
                puts("Unable to create vulkan buffer because the required memory type index was not found.");
                return false;
        }
        
        // Allocate memory info
        VkMemoryAllocateInfo allocate_info = {0};
        allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocate_info.pNext = NULL;
        allocate_info.allocationSize = requirements.size;
        allocate_info.memoryTypeIndex = (u32)out_buffer->memory_index;
        
        // Allocate the memory.
        VkResult result = vkAllocateMemory(
                                           context->device_context.logical_device,
                                           &allocate_info,
                                           context->instance_context.allocator,
                                           &out_buffer->memory);
        
        if (result != VK_SUCCESS) {
                printf("Unable to create vulkan buffer because the required memory allocation failed. Error: %i", result);
                return false;
        }
        
        if (bind_on_create) {
                vulkan_buffer_bind(context, out_buffer, 0);
        }
        puts("create buffers");
        
        return true;
}

bool create_buffers(vulkan_types* context) {
        VkMemoryPropertyFlagBits memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        
        const u64 vertex_buffer_size = sizeof(vertex_3d) * 1024 * 1024;
        if (!vulkan_buffer_create(
                                  context,
                                  vertex_buffer_size,
                                  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                  memory_property_flags,
                                  true,
                                  &context->buffer_context.object_vertex_buffer)) {
                puts("Error creating vertex buffer.");
                return false;
        }
        context->buffer_context.geometry_vertex_offset = 0;
        
        const u64 index_buffer_size = sizeof(u32) * 1024 * 1024;
        if (!vulkan_buffer_create(
                                  context,
                                  index_buffer_size,
                                  VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                  memory_property_flags,
                                  true,
                                  &context->buffer_context.object_index_buffer)) {
                puts("Error creating vertex buffer.");
                return false;
        }
        context->buffer_context.geometry_index_offset = 0;
        
        return true;
}

