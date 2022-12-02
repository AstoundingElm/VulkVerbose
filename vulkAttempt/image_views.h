#pragma once
#include "vulkan_types.h"

void vulkan_image_view_create(
                              vulkan_types* context,
                              VkFormat format,
                              vulkan_image* image,
                              VkImageAspectFlags aspect_flags) {
        VkImageViewCreateInfo view_create_info = {0};
        view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_create_info.pNext = NULL;
        view_create_info.image = image->handle;
        view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;  
        view_create_info.format = format;
        view_create_info.subresourceRange.aspectMask = aspect_flags;
        
        view_create_info.subresourceRange.baseMipLevel = 0;
        view_create_info.subresourceRange.levelCount = 1;
        view_create_info.subresourceRange.baseArrayLayer = 0;
        view_create_info.subresourceRange.layerCount = 1;
        
        VK_CHECK(vkCreateImageView(context->device_context.logical_device, &view_create_info, context->instance_context.allocator, &image->view));
}

PINLINE  bool image_create(vulkan_types* context, VkImageType image_type, u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags mem_flags, b32 create_view, VkImageAspectFlags view_aspect_flags, vulkan_image* out_image, VkSharingMode sharing_mode)
{
        
        out_image->width = width;
        out_image->height = height;
        
        // Creation info.
        VkImageCreateInfo image_create_info = {0};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.pNext = NULL;
        image_create_info.imageType = image_type;//VK_IMAGE_TYPE_2D;
        image_create_info.extent.width = width;
        image_create_info.extent.height = height;
        image_create_info.extent.depth = 1;  // TODO: Support configurable depth.
        image_create_info.mipLevels = 4;     // TODO: Support mip mapping
        image_create_info.arrayLayers = 1;   // TODO: Support number of layers in the image.
        image_create_info.format = format;
        image_create_info.tiling = tiling;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_create_info.usage = usage;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;          // TODO: Configurable sample count.
        image_create_info.sharingMode = sharing_mode;  // TODO: Configurable sharing mode.
        
        VK_CHECK(vkCreateImage(context->device_context.logical_device, &image_create_info, context->instance_context.allocator, &out_image->handle));
        
        // Query memory requirements.
        VkMemoryRequirements memory_requirements;
        vkGetImageMemoryRequirements(context->device_context.logical_device, out_image->handle, &memory_requirements);
        
        i32 memory_type = find_memory_index(memory_requirements.memoryTypeBits, mem_flags, context);
        if (memory_type == -1) {
                printf("Required memory type not found. Image not valid.");
        }
        
        // Allocate memory
        VkMemoryAllocateInfo memory_allocate_info = {0};
        memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memory_allocate_info.pNext = NULL;
        memory_allocate_info.allocationSize = memory_requirements.size;
        memory_allocate_info.memoryTypeIndex = memory_type;
        VK_CHECK(vkAllocateMemory(context->device_context.logical_device, &memory_allocate_info, context->instance_context.allocator, &out_image->memory));
        
        // Bind the memory
        VK_CHECK(vkBindImageMemory(context->device_context.logical_device, out_image->handle, out_image->memory, 0));  // TODO: configurable memory offset.
        
        // Create view
        
        if (create_view) {
                out_image->view = 0;
                vulkan_image_view_create(context, format, out_image, view_aspect_flags);
        }
        return true;
}

PINLINE bool get_swapchain_image_views(vulkan_types* context)
{
        //context->current_frame = 0;
        
        context->swapchain_context.swapchain_image_count = 0;
        
        context->swapchain_context.swapchain_images = malloc(sizeof(VkImage) * context->swapchain_context.swapchain_image_count);
        context->swapchain_context.swapchain_image_views = malloc(sizeof(VkImageView) * context->swapchain_context.swapchain_image_count);
        
        VK_CHECK(vkGetSwapchainImagesKHR(context->device_context.logical_device, context->swapchain_context.swapchain, &context->swapchain_context.swapchain_image_count, 0));
        VK_CHECK(vkGetSwapchainImagesKHR(context->device_context.logical_device, context->swapchain_context.swapchain, &context->swapchain_context.swapchain_image_count, context->swapchain_context.swapchain_images));
        
        printf("Image count: %d\n", context->swapchain_context.swapchain_image_count);
        for(u32 i = 0; i < context->swapchain_context.swapchain_image_count; i++)
        {
                VkImageViewCreateInfo view_info = {0};
                view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                view_info.pNext = NULL;
                view_info.flags = 0;
                view_info.image = context->swapchain_context.swapchain_images[i];
                view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
                view_info.format = context->swapchain_context.swap_format.format;
                /*view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
                */
                view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                view_info.subresourceRange.baseMipLevel = 0;
                view_info.subresourceRange.levelCount = 1;
                view_info.subresourceRange.baseArrayLayer = 0;
                view_info.subresourceRange.layerCount = 1;
                
                VK_CHECK(vkCreateImageView(context->device_context.logical_device, &view_info, NULL, &context->swapchain_context.swapchain_image_views[i]));
                
        }
        puts("created swapchain image views\n");
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
                vkGetPhysicalDeviceFormatProperties(context->device_context.gpu, candidates[i], &properties);
                if ((properties.linearTilingFeatures & flags) == flags) {
                        context->depth_context.depth_format = candidates[i];
                        break;
                } else if ((properties.optimalTilingFeatures & flags) == flags) {
                        context->depth_context.depth_format = candidates[i];
                        break;
                        
                }
        }
        //create the swapchain image  view, then
        // create depth image and view
        
        image_create(context, VK_IMAGE_TYPE_2D, context->swapchain_context.swapchain_extent.width,context->swapchain_context.swapchain_extent.height, context->depth_context.depth_format,  VK_IMAGE_TILING_OPTIMAL,
                     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     true,
                     VK_IMAGE_ASPECT_DEPTH_BIT, &context->depth_context.depth_attachment, VK_SHARING_MODE_EXCLUSIVE);
        puts("created depth image and view  attachment\n");
        
        return true;
        
}




