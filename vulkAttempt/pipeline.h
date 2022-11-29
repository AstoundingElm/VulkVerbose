#pragma once
//@param cunt

#define OBJECT_SHADER_STAGE_COUNT 2

PINLINE bool create_shader_module(vulkan_types* context, const char * name, const char * type_str, VkShaderStageFlagBits shader_stage_flag,
                                  u32 stage_index,
                                  vulkan_shader_stage* shader_stages)
{
        
        read_shader_file();
        
        kzero_memory(&shader_stages[stage_index].create_info, sizeof(VkShaderModuleCreateInfo));
        shader_stages[stage_index].create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        
        shader_stages[stage_index].create_info.codeSize = size;
        shader_stages[stage_index].create_info.pCode = (u32*)file_buffer;
        
        
        
        VK_CHECK(vkCreateShaderModule(
                                      context->device.logical_device,
                                      &shader_stages[stage_index].create_info,
                                      context->allocator,
                                      &shader_stages[stage_index].handle));
        
        // Shader stage info
        kzero_memory(&shader_stages[stage_index].shader_stage_create_info, sizeof(VkPipelineShaderStageCreateInfo));
        shader_stages[stage_index].shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[stage_index].shader_stage_create_info.stage = shader_stage_flag;
        shader_stages[stage_index].shader_stage_create_info.module = shader_stages[stage_index].handle;
        shader_stages[stage_index].shader_stage_create_info.pName = "main";
        return true;
        
}


#define BUILTIN_SHADER_NAME_OBJECT "Builtin.ObjectShader"
PINLINE bool create_shaders(vulkan_types* context, vulkan_object_shader* out_shader)
{
        char stage_type_strs[OBJECT_SHADER_STAGE_COUNT][5] = {"vert", "frag"};
        VkShaderStageFlagBits stage_types[OBJECT_SHADER_STAGE_COUNT] = {VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};
        
        for (u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i) {
                if (!create_shader_module(context, BUILTIN_SHADER_NAME_OBJECT, stage_type_strs[i], stage_types[i], i, out_shader->stages)) {
                        printf("Unable to create %s shader module for '%s'.", stage_type_strs[i], BUILTIN_SHADER_NAME_OBJECT);
                        return false;
                }
        }
        
}

PINLINE bool create_pipeline(vulkan_types* context, bool is_wireframe)
{
        
        VkViewport viewport = {0};
        
        VkRect2D scissor = {0};
        
        VkPipelineViewportStateCreateInfo viewport_state = {0};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.pNext = 0;
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = &viewport;
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = &scissor;
        
        // Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizer_create_info = {0}; rasterizer_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer_create_info.pNext = NULL;
        rasterizer_create_info.depthClampEnable = VK_FALSE;
        rasterizer_create_info.rasterizerDiscardEnable = VK_FALSE;
        rasterizer_create_info.polygonMode = is_wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
        rasterizer_create_info.lineWidth = 1.0f;
        rasterizer_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer_create_info.depthBiasEnable = VK_FALSE;
        rasterizer_create_info.depthBiasConstantFactor = 0.0f;
        rasterizer_create_info.depthBiasClamp = 0.0f;
        rasterizer_create_info.depthBiasSlopeFactor = 0.0f;
        
        // Multisampling.
        VkPipelineMultisampleStateCreateInfo multisampling_create_info = {0};
        multisampling_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling_create_info.pNext = NULL;
        multisampling_create_info.sampleShadingEnable = VK_FALSE;
        multisampling_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling_create_info.minSampleShading = 1.0f;
        multisampling_create_info.pSampleMask = 0;
        multisampling_create_info.alphaToCoverageEnable = VK_FALSE;
        multisampling_create_info.alphaToOneEnable = VK_FALSE;
        
        
        // Depth and stencil testing.
        VkPipelineDepthStencilStateCreateInfo depth_stencil = {0};
        depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil.pNext = NULL;
        depth_stencil.depthTestEnable = VK_TRUE;
        depth_stencil.depthWriteEnable = VK_TRUE;
        depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depth_stencil.depthBoundsTestEnable = VK_FALSE;
        depth_stencil.stencilTestEnable = VK_FALSE;
        
        VkPipelineColorBlendAttachmentState color_blend_attachment_state;
        memset(&color_blend_attachment_state, 0, sizeof(VkPipelineColorBlendAttachmentState));
        color_blend_attachment_state.blendEnable = VK_TRUE;
        color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
        color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
        
        color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        
        VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {0};
        color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blend_state_create_info.pNext = NULL;
        color_blend_state_create_info.logicOpEnable = VK_FALSE;
        color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
        color_blend_state_create_info.attachmentCount = 1;
        color_blend_state_create_info.pAttachments = &color_blend_attachment_state;
        
        // Dynamic state
        const u32 dynamic_state_count = 3;
        VkDynamicState dynamic_states[3] = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR,
                VK_DYNAMIC_STATE_LINE_WIDTH};
        
        VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {0}; dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state_create_info.pNext  = NULL;
        dynamic_state_create_info.dynamicStateCount = dynamic_state_count;
        dynamic_state_create_info.pDynamicStates = dynamic_states;
        
        // Vertex input
        VkVertexInputBindingDescription binding_description;
        binding_description.binding = 0;  // Binding index
        binding_description.stride = sizeof(vertex_3d);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;  // Move to next data entry for each vertex.
        
        // Attributes
        VkPipelineVertexInputStateCreateInfo vertex_input_info = {0};
        vertex_input_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        
        vertex_input_info.pNext = NULL;
        vertex_input_info.vertexBindingDescriptionCount = 1;
        vertex_input_info.pVertexBindingDescriptions = &binding_description;
        vertex_input_info.vertexAttributeDescriptionCount = 1;
        vertex_input_info.pVertexAttributeDescriptions = context->input_attribute_context.input_attribute_descriptions;
        
        // Input assembly
        VkPipelineInputAssemblyStateCreateInfo input_assembly = {0};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.pNext = NULL;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;
        
        // Pipeline layout
        VkPipelineLayoutCreateInfo pipeline_layout_create_info = {0};
        pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        
        
        // Push constants
        VkPushConstantRange push_constant;
        push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        push_constant.offset = sizeof(mat4) * 0;
        push_constant.size = sizeof(mat4) * 2;
        pipeline_layout_create_info.pushConstantRangeCount = 1;
        pipeline_layout_create_info.pPushConstantRanges = &push_constant;
        
        // Descriptor set layouts
        pipeline_layout_create_info.setLayoutCount = 1;//descriptor_set_layout_count;
        pipeline_layout_create_info.pSetLayouts = context->descriptor_context.layouts;
        
        
        
        // Create the pipeline layout.
        VK_CHECK(vkCreatePipelineLayout(
                                        context->device_context.logical_device,
                                        &pipeline_layout_create_info,
                                        context->instance_context.allocator,
                                        &context->pipeline_context.pipe_layout));
        
        
        
        // Pipeline create
        VkGraphicsPipelineCreateInfo pipeline_create_info = {0};
        pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_create_info.pNext = NULL;
        pipeline_create_info.stageCount = 2;
        pipeline_create_info.pStages = shaderStages;
        pipeline_create_info.pVertexInputState = &vertex_input_info;
        pipeline_create_info.pInputAssemblyState = &input_assembly;
        
        pipeline_create_info.pViewportState = &viewport_state;
        pipeline_create_info.pRasterizationState = &rasterizer_create_info;
        pipeline_create_info.pMultisampleState = &multisampling_create_info;
        pipeline_create_info.pDepthStencilState = &depth_stencil;
        pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
        pipeline_create_info.pDynamicState = &dynamic_state_create_info;
        pipeline_create_info.pTessellationState = 0;
        
        pipeline_create_info.layout = context->pipeline_context.pipe_layout;
        
        pipeline_create_info.renderPass = context->renderpass_context.renderpass;
        pipeline_create_info.subpass = 0;
        pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
        pipeline_create_info.basePipelineIndex = -1;
        
        VK_CHECK(vkCreateGraphicsPipelines(
                                           context->device_context.logical_device,
                                           VK_NULL_HANDLE,
                                           1,
                                           &pipeline_create_info,
                                           context->instance_context.allocator,
                                           &context->pipeline_context.pipeline));
        puts("successfully created pipeline\n");
        return true;
        
}
