#pragma once
//@param cunt

#include "shader_utils.h"

#define OBJECT_SHADER_STAGE_COUNT 2

i32 string_format_v(char* dest, const char* format, void* va_listp) {
        if (dest) {
                // Big, but can fit on the stack.
                char buffer[32000];
                i32 written = vsnprintf(buffer, 32000, format, va_listp);
                buffer[written] = 0;
                memcpy(dest, buffer, written + 1);
                
                return written;
        }
        return -1;
}

i32 string_format(char* dest, const char* format, ...) {
        if (dest) {
                __builtin_va_list arg_ptr;
                va_start(arg_ptr, format);
                i32 written = string_format_v(dest, format, arg_ptr);
                va_end(arg_ptr);
                return written;
        }
        return -1;
}


PINLINE bool create_shader_module(vulkan_types* context, const char * name, const char * type_str, VkShaderStageFlagBits shader_stage_flag,
                                  u32 stage_index,
                                  vulkan_shader_stage* shader_stages)
{
        
        char file_name[512];
        string_format(file_name, "/home/petermiller/Desktop/4coder/vulkAttempt/%s.%s.spv", name, type_str);
        
        
        shader_file_info shader_module_data = {0};
        
        memset(&shader_stages[stage_index].create_info, 0,  sizeof(VkShaderModuleCreateInfo));
        shader_stages[stage_index].create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        
        shader_module_data.file_handle = NULL;
        
        shader_module_data.file_handle = fopen(file_name, "rb+");
        shader_module_data.shader_loaded = true;
        if (shader_module_data.file_handle == NULL)  {
                shader_module_data.shader_loaded = false;
                printf("could loadnt spv files");
                return false;
        };
        
        fseek(shader_module_data.file_handle, 0, SEEK_END);
        
        shader_module_data.code_size = ftell(shader_module_data.file_handle);
        
        
        shader_module_data.code = malloc(shader_module_data.code_size * sizeof(u8 ));
        
        
        rewind(shader_module_data.file_handle);
        
        fread(shader_module_data.code, 1, shader_module_data.code_size, shader_module_data.file_handle);
        
        fclose(shader_module_data.file_handle);
        
        shader_stages[stage_index].create_info.codeSize = shader_module_data.code_size;
        shader_stages[stage_index].create_info.pCode =
        (u32*)shader_module_data.code;
        
        VK_CHECK(vkCreateShaderModule(
                                      context->device_context.logical_device,
                                      &shader_stages[stage_index].create_info,
                                      context->instance_context.allocator,
                                      &shader_stages[stage_index].handle));
        
        // Shader stage info
        memset(&shader_stages[stage_index].shader_stage_create_info,0, sizeof(VkPipelineShaderStageCreateInfo));
        shader_stages[stage_index].shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[stage_index].shader_stage_create_info.stage = shader_stage_flag;
        shader_stages[stage_index].shader_stage_create_info.module = shader_stages[stage_index].handle;
        shader_stages[stage_index].shader_stage_create_info.pName = "main";
        return true;
        
}

#define BUILTIN_SHADER_NAME_OBJECT "Pshader"

PINLINE bool create_object_shader(vulkan_types* context, bool is_wireframe, vulkan_object_shader* out_shader)
{
        char stage_type_strs[OBJECT_SHADER_STAGE_COUNT][5] = {"vert", "frag"};
        VkShaderStageFlagBits stage_types[OBJECT_SHADER_STAGE_COUNT] = {VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};
        
        for (u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i) {
                if (!create_shader_module(context, BUILTIN_SHADER_NAME_OBJECT, stage_type_strs[i], stage_types[i], i, out_shader->stages)) {
                        printf("Unable to create %s shader module for '%s'.", stage_type_strs[i], BUILTIN_SHADER_NAME_OBJECT);
                        return false;
                }
        }
        
        // Global Descriptors
        VkDescriptorSetLayoutBinding global_ubo_layout_binding;
        global_ubo_layout_binding.binding = 0;
        global_ubo_layout_binding.descriptorCount = 1;
        global_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        global_ubo_layout_binding.pImmutableSamplers = 0;
        global_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        
        VkDescriptorSetLayoutCreateInfo global_layout_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        global_layout_info.bindingCount = 1;
        global_layout_info.pBindings = &global_ubo_layout_binding;
        VK_CHECK(vkCreateDescriptorSetLayout(context->device_context.logical_device, &global_layout_info, context->instance_context.allocator, &out_shader->global_descriptor_set_layout));
        
        // Global descriptor pool: Used for global items such as view/projection matrix.
        VkDescriptorPoolSize global_pool_size;
        global_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        global_pool_size.descriptorCount = context->swapchain_context.swapchain_image_count;
        
        VkDescriptorPoolCreateInfo global_pool_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
        global_pool_info.poolSizeCount = 1;
        global_pool_info.pPoolSizes = &global_pool_size;
        global_pool_info.maxSets = 100;//context->swapchain.image_count;
        VK_CHECK(vkCreateDescriptorPool(context->device_context.logical_device, &global_pool_info, context->instance_context.allocator, &out_shader->global_descriptor_pool));
        
        // Pipeline creation
        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = (f32)context->framebuffer_context.framebuffer_height;
        viewport.width = (f32)context->framebuffer_context.framebuffer_width;
        viewport.height = -(f32)context->framebuffer_context.framebuffer_height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        
        // Scissor
        VkRect2D scissor;
        scissor.offset.x = scissor.offset.y = 0;
        scissor.extent.width = context->framebuffer_context.framebuffer_width;
        scissor.extent.height = context->framebuffer_context.framebuffer_height;
        
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
        
        
        // Desciptor set layouts.
        const i32 descriptor_set_layout_count = 1;
        VkDescriptorSetLayout layouts[1] = {
                out_shader->global_descriptor_set_layout};
        
        // Stages
        // NOTE: Should match the number of shader->stages.
        VkPipelineShaderStageCreateInfo stage_create_infos[OBJECT_SHADER_STAGE_COUNT];
        memset(stage_create_infos, 0, sizeof(stage_create_infos));
        for (u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i) {
                stage_create_infos[i].sType = out_shader->stages[i].shader_stage_create_info.sType;
                stage_create_infos[i] = out_shader->stages[i].shader_stage_create_info;
        }
        
        VkPipelineViewportStateCreateInfo viewport_state = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = &viewport;
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = &scissor;
        
        // Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizer_create_info = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
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
        VkPipelineMultisampleStateCreateInfo multisampling_create_info = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
        multisampling_create_info.sampleShadingEnable = VK_FALSE;
        multisampling_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling_create_info.minSampleShading = 1.0f;
        multisampling_create_info.pSampleMask = 0;
        multisampling_create_info.alphaToCoverageEnable = VK_FALSE;
        multisampling_create_info.alphaToOneEnable = VK_FALSE;
        
        // Depth and stencil testing.
        VkPipelineDepthStencilStateCreateInfo depth_stencil = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
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
        
        VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
        color_blend_state_create_info.logicOpEnable = VK_FALSE;
        color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
        color_blend_state_create_info.attachmentCount = 1;
        color_blend_state_create_info.pAttachments = &color_blend_attachment_state;
        
        // Dynamic state
        const u32 dynamic_state_count = 3;
        VkDynamicState dynamic_states[dynamic_state_count] = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR,
                VK_DYNAMIC_STATE_LINE_WIDTH};
        
        VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
        dynamic_state_create_info.dynamicStateCount = dynamic_state_count;
        dynamic_state_create_info.pDynamicStates = dynamic_states;
        
        // Vertex input
        VkVertexInputBindingDescription binding_description;
        binding_description.binding = 0;  // Binding index
        binding_description.stride = sizeof(vertex_3d);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;  // Move to next data entry for each vertex.
        
        // Attributes
        VkPipelineVertexInputStateCreateInfo vertex_input_info = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
        vertex_input_info.vertexBindingDescriptionCount = 1;
        vertex_input_info.pVertexBindingDescriptions = &binding_description;
        vertex_input_info.vertexAttributeDescriptionCount = attribute_count;
        vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions;
        
        // Input assembly
        VkPipelineInputAssemblyStateCreateInfo input_assembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;
        
        // Pipeline layout
        VkPipelineLayoutCreateInfo pipeline_layout_create_info = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        
        // Push constants
        VkPushConstantRange push_constant;
        push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        push_constant.offset = sizeof(mat4) * 0;
        push_constant.size = sizeof(mat4) * 2;
        pipeline_layout_create_info.pushConstantRangeCount = 1;
        pipeline_layout_create_info.pPushConstantRanges = &push_constant;
        
        // Descriptor set layouts
        pipeline_layout_create_info.setLayoutCount = descriptor_set_layout_count;
        pipeline_layout_create_info.pSetLayouts = layouts;
        
        // Create the pipeline layout.
        VK_CHECK(vkCreatePipelineLayout(
                                        context->device_context.logical_device,
                                        &pipeline_layout_create_info,
                                        context->instance_context.allocator,
                                        &out_shader->pipeline.pipeline_layout));
        
        // Pipeline create
        VkGraphicsPipelineCreateInfo pipeline_create_info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
        pipeline_create_info.stageCount = OBJECT_SHADER_STAGE_COUNT;
        pipeline_create_info.pStages = stage_create_infos;
        pipeline_create_info.pVertexInputState = &vertex_input_info;
        pipeline_create_info.pInputAssemblyState = &input_assembly;
        
        pipeline_create_info.pViewportState = &viewport_state;
        pipeline_create_info.pRasterizationState = &rasterizer_create_info;
        pipeline_create_info.pMultisampleState = &multisampling_create_info;
        pipeline_create_info.pDepthStencilState = &depth_stencil;
        pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
        pipeline_create_info.pDynamicState = &dynamic_state_create_info;
        pipeline_create_info.pTessellationState = 0;
        
        pipeline_create_info.layout = out_shader->pipeline.pipeline_layout;
        
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
                                           &out_shader->pipeline.handle));
        
        
        return true;
}


void old(){
        
        /*
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
        puts("successfully created pipeline\n");*/
}
