#pragma once
#include "platform_types.h"

PINLINE bool create_framebuffers(vulkan_types* context, platform_state* platform_context)
{
        u32 attachment_count = 2;
        //context->image_count = 5;
        
        for(u32 i = 0; i < context->swapchain_context.swapchain_image_count; i++){
                VkImageView attachments[] = {
                        context->swapchain_context.swapchain_image_views[i],
                        context->depth_context.depth_attachment.view,
                };
                
                context->attachment_context.attachments = malloc(sizeof(VkImageView)*attachment_count);
                for(u32 i = 0; i < attachment_count; i++){
                        context->attachment_context.attachments[i] = attachments[i];
                }
                
                context->attachment_context.attachment_count = attachment_count;
                
                VkFramebufferCreateInfo framebuffer_info = {0};
                framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebuffer_info.renderPass = context->renderpass_context.renderpass;
                framebuffer_info.attachmentCount = context->attachment_context.attachment_count;
                framebuffer_info.pAttachments = context->attachment_context.attachments;
                framebuffer_info.width = platform_context->win_width;
                framebuffer_info.height = platform_context->win_height;
                framebuffer_info.layers = 1;
                
                VK_CHECK(vkCreateFramebuffer(
                                             context->device_context.logical_device,
                                             &framebuffer_info,
                                             context->instance_context.allocator,
                                             &context->framebuffer_context.framebuffer));
                
        }
        
        return true;
}

