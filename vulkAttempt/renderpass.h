#pragma once

PINLINE bool create_render_pass(vulkan_types* context)
{
        VkSubpassDescription subpass= {0};
        
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        
        const u32 attachment_description_count = 2;
        
        VkAttachmentDescription attachment_descriptions[2];
        
        
        VkAttachmentDescription colour_attachment = {0};
        colour_attachment.format = context->swapchain_context.swap_format.format;
        colour_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colour_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colour_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colour_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colour_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        
        colour_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colour_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        colour_attachment.flags = 0;
        
        attachment_descriptions[0] = colour_attachment;
        
        VkAttachmentReference colorAttachmentRef = {0};
        colorAttachmentRef.attachment = 0; // This is an index into the attachments array
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        
        VkAttachmentDescription depth_attachment = {0};
        
        depth_attachment.format = context->depth_context.depth_format;
        depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        
        attachment_descriptions[1] = depth_attachment;
        
        // Depth attachment reference
        VkAttachmentReference depth_attachment_reference = {0};
        depth_attachment_reference.attachment = 1;
        depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        
        // TODO: other attachment types (input, resolve, preserve)
        
        // Depth stencil data.
        subpass.pDepthStencilAttachment = &depth_attachment_reference;
        
        // Input from a shader
        subpass.inputAttachmentCount = 0;
        subpass.pInputAttachments = 0;
        
        // Attachments used for multisampling colour attachments
        subpass.pResolveAttachments = 0;
        
        // Attachments not used in this subpass, but must be preserved for the next.
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments = 0;
        
        // Render pass dependencies. TODO: make this configurable.
        VkSubpassDependency dependency;
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dependencyFlags = 0;
        
        // Render pass create.
        VkRenderPassCreateInfo render_pass_create_info = {0};
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.attachmentCount = attachment_description_count;
        render_pass_create_info.pAttachments = attachment_descriptions;
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &subpass;
        render_pass_create_info.dependencyCount = 1;
        render_pass_create_info.pDependencies = &dependency;
        render_pass_create_info.pNext = 0;
        render_pass_create_info.flags = 0;
        
        VK_CHECK(vkCreateRenderPass(
                                    context->device_context.logical_device,
                                    &render_pass_create_info,
                                    context->instance_context.allocator,
                                    &context->renderpass_context.renderpass));
        puts("Renderpass created succesffuly\n");
        return true;
        
}
