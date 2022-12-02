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
                        bool bind_on_create,
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
        VK_CHECK(vkAllocateMemory(
                                  context->device_context.logical_device,
                                  &allocate_info,
                                  context->instance_context.allocator,
                                  &out_buffer->memory));
        
        
        if (bind_on_create) {
                VK_CHECK(vkBindBufferMemory(context->device_context.logical_device, out_buffer->handle, out_buffer->memory, 0));
                //vulkan_buffer_bind(context, out_buffer, 0);
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
                                  &context->object_vertex_buffer)) {
                puts("Error creating vertex buffer.");
                return false;
        }
        context->geometry_vertex_offset = 0;
        
        const u64 index_buffer_size = sizeof(u32) * 1024 * 1024;
        if (!vulkan_buffer_create(
                                  context,
                                  index_buffer_size,
                                  VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                  memory_property_flags,
                                  true,
                                  &context->object_index_buffer)) {
                puts("Error creating vertex buffer.");
                return false;
        }
        context->geometry_index_offset = 0;
        
        return true;
}

void vulkan_buffer_load_data(vulkan_types* context, vulkan_buffer* buffer, u64 offset, u64 size, u32 flags, const void* data) {
        void* data_ptr[8000];
        VK_CHECK(vkMapMemory(context->device_context.logical_device, buffer->memory, offset, size, flags, data_ptr));
        memcpy(data_ptr, data, size);
        vkUnmapMemory(context->device_context.logical_device, buffer->memory);
}

void vulkan_buffer_copy_to(
                           vulkan_types* context,
                           VkCommandPool pool,
                           VkFence fence,
                           VkQueue queue,
                           VkBuffer source,
                           u64 source_offset,
                           VkBuffer dest,
                           u64 dest_offset,
                           u64 size) {
        vkQueueWaitIdle(queue);
        // Create a one-time-use command buffer.
        vulkan_command_buffer temp_command_buffer;
        vulkan_command_buffer_allocate_and_begin_single_use(context, pool, &temp_command_buffer);
        
        // Prepare the copy command and add it to the command buffer.
        VkBufferCopy copy_region;
        copy_region.srcOffset = source_offset;
        copy_region.dstOffset = dest_offset;
        copy_region.size = size;
        
        vkCmdCopyBuffer(temp_command_buffer.handle, source, dest, 1, &copy_region);
        
        // Submit the buffer for execution and wait for it to complete.
        vulkan_command_buffer_end_single_use(context, pool, &temp_command_buffer, queue);
}

void vulkan_buffer_destroy(vulkan_types* context, vulkan_buffer* buffer) {
        if (buffer->memory) {
                vkFreeMemory(context->device_context.logical_device, buffer->memory, context->instance_context.allocator);
                buffer->memory = 0;
        }
        if (buffer->handle) {
                vkDestroyBuffer(context->device_context.logical_device, buffer->handle, context->instance_context.allocator);
                buffer->handle = 0;
        }
        buffer->total_size = 0;
        buffer->usage = 0;
        buffer->is_locked = false;
}

void upload_data_range(vulkan_types* context, VkCommandPool pool, VkFence fence, VkQueue queue, vulkan_buffer* buffer, u64 offset, u64 size, void* data) {
        // Create a host-visible staging buffer to upload to. Mark it as the source of the transfer.
        VkBufferUsageFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        vulkan_buffer staging;
        vulkan_buffer_create(context, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, flags, true, &staging);
        
        // Load the data into the staging buffer.
        vulkan_buffer_load_data(context, &staging, 0, size, 0, data);
        
        // Perform the copy from staging to the device local buffer.
        vulkan_buffer_copy_to(context, pool, fence, queue, staging.handle, 0, buffer->handle, offset, size);
        
        // Clean up the staging buffer.
        vulkan_buffer_destroy(context, &staging);
}

bool upload_data_ranges(vulkan_types*context){
        
        const u32 vert_count = 4;
        vertex_3d verts[4];
        memset(verts, 0,sizeof(vertex_3d) * vert_count);
        
        const f32 f = 10.0f;
        
        verts[0].position.x = -0.5 * f;
        verts[0].position.y = -0.5 * f;
        
        verts[1].position.y = 0.5 * f;
        verts[1].position.x = 0.5 * f;
        
        verts[2].position.x = -0.5 * f;
        verts[2].position.y = 0.5 * f;
        
        verts[3].position.x = 0.5 * f;
        verts[3].position.y = -0.5 * f;
        
        const u32 index_count = 6;
        u32 indices[6] = {0, 1, 2, 0, 3, 1};
        
        upload_data_range(context, context->command_pool_context.primaryCommandPool, 0, context->queue_context.graphics_queue, &context->object_vertex_buffer, 0, sizeof(vertex_3d) * vert_count, verts);
        upload_data_range(context, context->command_pool_context.primaryCommandPool, 0, context->queue_context.graphics_queue, &context->object_index_buffer, 0, sizeof(u32) * index_count, indices);
        // TODO: end temp code
        
        puts("Vulkan renderer initialized successfully.");
        return true;
};

