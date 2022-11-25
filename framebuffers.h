#pragma once


PINLINE bool create_framebuffers(vulkan_types* context)
{
        VkFramebufferCreateInfo framebuffer_info = {0};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = context->renderpass;
        framebuffer_info.attachmentCount = context->attachment_count;
        framebuffer_info.pAttachments = &context->attachments;
        framebuffer_info.width = state.win_width;
        framebuffer_info.height = state.win_height;
        framebuffer_info.layers = 1;
        
        VK_CHECK(vkCreateFramebuffer(
                                     context->logical_device,
                                     &framebuffer_info,
                                     context->allocator,
                                     &context->framebuffer));
        return true;
}

