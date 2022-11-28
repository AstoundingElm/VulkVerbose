#pragma once

PINLINE bool create_descriptor_layouts(vulkan_types* context)
{
        VkDescriptorSetLayoutBinding global_ubo_binding = {0};
        global_ubo_binding.binding  = 0;
        global_ubo_binding.descriptorCount = 1;
        global_ubo_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        global_ubo_binding.pImmutableSamplers = 0;
        global_ubo_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        
        VkDescriptorSetLayoutCreateInfo layout_info = {0};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = 1;
        layout_info.pBindings = &global_ubo_binding;
        
        VK_CHECK(vkCreateDescriptorSetLayout(context->logical_device, &layout_info, context->allocator, &context->set_layout));
        // Global descriptor pool: Used for global items such as view/projection matrix.
        VkDescriptorPoolSize global_pool_size;
        global_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        global_pool_size.descriptorCount = context->image_count;
        
        VkDescriptorPoolCreateInfo global_pool_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
        global_pool_info.poolSizeCount = 1;
        global_pool_info.pPoolSizes = &global_pool_size;
        global_pool_info.maxSets = 100;//context->swapchain.image_count;
        VK_CHECK(vkCreateDescriptorPool(context->logical_device, &global_pool_info, context->allocator, &context->global_descriptor_pool));
        
        // Pipeline creation
        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = (f32)context->framebuffer_height;
        viewport.width = (f32)context->framebuffer_width;
        viewport.height = -(f32)context->framebuffer_height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        
        // Scissor
        VkRect2D scissor;
        scissor.offset.x = scissor.offset.y = 0;
        scissor.extent.width = context->framebuffer_width;
        scissor.extent.height = context->framebuffer_height;
        
        // Attributes
        u32 offset = 0;
        const i32 attribute_count = 1;
        VkVertexInputAttributeDescription attribute_descriptions[attribute_count];
        // Position
        VkFormat formats[attribute_count] = {
                VK_FORMAT_R32G32B32_SFLOAT};
        u64 sizes[attribute_count] = {
                sizeof(vec3)};
        for (u32 i = 0; i < attribute_count; ++i) {
                attribute_descriptions[i].binding = 0;   // binding index - should match binding desc
                attribute_descriptions[i].location = i;  // attrib location
                attribute_descriptions[i].format = formats[i];
                attribute_descriptions[i].offset = offset;
                offset += sizes[i];
        }
        
        for(uint32_t i = 0; i <  attribute_count; i++){
                context->input_attribute_descriptions[i] = attribute_descriptions[i];
        }
        
        // Desciptor set layouts.
        const i32 descriptor_set_layout_count = 1;
        VkDescriptorSetLayout layouts[1] = {
                context->global_descriptor_set_layout};
        
        /*// Stages
        // NOTE: Should match the number of shader->stages.
        VkPipelineShaderStageCreateInfo stage_create_infos[OBJECT_SHADER_STAGE_COUNT];
        kzero_memory(stage_create_infos, sizeof(stage_create_infos));
        for (u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i) {
                stage_create_infos[i].sType = out_shader->stages[i].shader_stage_create_info.sType;
                stage_create_infos[i] = out_shader->stages[i].shader_stage_create_info;
        }
        */
        printf("successfully created descriptor layouts\n");
        return true;
}
