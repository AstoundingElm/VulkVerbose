#pragma once

#include "defines.h"
#include "vulkan_types.h"

void vulkan_command_buffer_allocate(
                                    vulkan_types* context,
                                    VkCommandPool pool,
                                    b8 is_primary,
                                    vulkan_command_buffer* out_command_buffer) {
        
        memset(out_command_buffer, 0, sizeof(*out_command_buffer));
        
        VkCommandBufferAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        allocate_info.commandPool = pool;
        allocate_info.level = is_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocate_info.commandBufferCount = 1;
        allocate_info.pNext = 0;
        
        out_command_buffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
        VK_CHECK(vkAllocateCommandBuffers(
                                          context->logical_device,
                                          &allocate_info,
                                          &out_command_buffer->handle));
        out_command_buffer->state = COMMAND_BUFFER_STATE_READY;
}

void vulkan_command_buffer_free(
                                vulkan_types* context,
                                VkCommandPool pool,
                                vulkan_command_buffer* command_buffer) {
        vkFreeCommandBuffers(
                             context->logical_device,
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
        
        VkCommandBufferBeginInfo begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
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

void vulkan_buffer_destroy(vulkan_types* context, vulkan_buffer* buffer) {
        if (buffer->memory) {
                vkFreeMemory(context->logical_device, buffer->memory, context->allocator);
                buffer->memory = 0;
        }
        if (buffer->handle) {
                vkDestroyBuffer(context->logical_device, buffer->handle, context->allocator);
                buffer->handle = 0;
        }
        buffer->total_size = 0;
        buffer->usage = 0;
        buffer->is_locked = false;
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

void* vulkan_buffer_lock_memory(vulkan_types* context, vulkan_buffer* buffer, u64 offset, u64 size, u32 flags) {
        void* data;
        VK_CHECK(vkMapMemory(context->logical_device, buffer->memory, offset, size, flags, &data));
        return data;
}

void vulkan_buffer_unlock_memory(vulkan_types* context, vulkan_buffer* buffer) {
        vkUnmapMemory(context->logical_device, buffer->memory);
}

void vulkan_buffer_load_data(vulkan_types* context, vulkan_buffer* buffer, u64 offset, u64 size, u32 flags, const void* data) {
        void* data_ptr;
        VK_CHECK(vkMapMemory(context->logical_device, buffer->memory, offset, size, flags, &data_ptr));
        memcpy(data_ptr, data, size);
        vkUnmapMemory(context->logical_device, buffer->memory);
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

bool init(vulkan_types context){
        // TODO: temporary test code
        const u32 vert_count = 4;
        vertex_3d verts[vert_count];
        memset(verts, 0, sizeof(vertex_3d) * vert_count);
        
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
        u32 indices[index_count] = {0, 1, 2, 0, 3, 1};
        
        upload_data_range(&context, context.primaryCommandPool, 0, context.graphics_queue, &context.object_vertex_buffer, 0, sizeof(vertex_3d) * vert_count, verts);
        upload_data_range(&context, context.primaryCommandPool, 0, context.graphics_queue, &context.object_index_buffer, 0, sizeof(u32) * index_count, indices);
        // TODO: end temp code
        
        puts("Vulkan renderer initialized successfully.");
        return true;
}

bool vulkan_object_shader_update_global_state(vulkan_types* context,  vulkan_object_shader* shader) {
        u32 image_index = context->image_index;
        VkCommandBuffer command_buffer = context->graphics_command_buffer[image_index].handle;
        VkDescriptorSet global_descriptor = shader->global_descriptor_sets[image_index];
        
        // Configure the descriptors for the given index.
        u32 range = sizeof(global_uniform_object);
        u64 offset = 0;//sizeof(global_uniform_object) * image_index;
        
        // Copy data to buffer
        vulkan_buffer_load_data(context, &shader->global_uniform_buffer, offset, range, 0, &shader->global_ubo);
        
        VkDescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = shader->global_uniform_buffer.handle;
        bufferInfo.offset = offset;
        bufferInfo.range = range;
        
        // Update descriptor sets.
        VkWriteDescriptorSet descriptor_write = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        descriptor_write.dstSet = shader->global_descriptor_sets[image_index];
        descriptor_write.dstBinding = 0;
        descriptor_write.dstArrayElement = 0;
        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_write.descriptorCount = 1;
        descriptor_write.pBufferInfo = &bufferInfo;
        vkUpdateDescriptorSets(context->logical_device, 1, &descriptor_write, 0, 0);
        // Bind the global descriptor set to be updated.
        // 
        return true;
}