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
        
        return true;
};
