#pragma once


PINLINE bool get_image_views(vulkan_types* context)
{
        context->image_count = 3;
        VK_CHECK(vkGetSwapchainImagesKHR(context->logical_device, context->swapchain, &context->image_count, NULL));
        VK_CHECK(vkGetSwapchainImagesKHR(context->logical_device, context->swapchain, &context->image_count, context->swapchain_images));
        for(int i = 0; i < context->image_count; i++)
        {
                VkImageViewCreateInfo view_info = {0};
                view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                view_info.pNext = NULL;
                view_info.flags = 0;
                view_info.image = context->swapchain_images[i];
                view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
                view_info.format = context->present_format;
                view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
                view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                view_info.subresourceRange.baseMipLevel = 0;
                view_info.subresourceRange.levelCount = 1;
                view_info.subresourceRange.baseArrayLayer = 0;
                view_info.subresourceRange.layerCount = 1;
                
                VK_CHECK(vkCreateImageView(context->logical_device, &view_info, NULL, &context->swapchain_image_views[i]));
                
        }
        
        const u64 candidate_count = 3;
        VkFormat candidates[3] = 
        {
                VK_FORMAT_D32_SFLOAT,
                VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D24_UNORM_S8_UINT,
        };
        
        u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
        for(u64 i = 0; i < candidate_count; i++)
        {
                VkFormatProperties properties;
                vkGetPhysicalDeviceFormatProperties(context->gpu, candidates[i], &properties);
                if ((properties.linearTilingFeatures & flags) == flags) {
                        context->depth_format = candidates[i];
                        return true;
                } else if ((properties.optimalTilingFeatures & flags) == flags) {
                        context->depth_format = candidates[i];
                        return true;
                }
        }
        
        return false;
        
}
