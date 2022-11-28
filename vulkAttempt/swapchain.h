#pragma once

static const char * pmode_to_str(VkPresentModeKHR mode)
{
        static char buf[8000];
        switch(mode)
        {
                case VK_PRESENT_MODE_IMMEDIATE_KHR: return "VK_PRESENT_MODE_IMMEDIATE";
		case VK_PRESENT_MODE_MAILBOX_KHR: return "VK_PRESENT_MODE_MAILBOX_KHR";
		case VK_PRESENT_MODE_FIFO_KHR: return "VK_PRESENT_MODE_FIFO";
		case VK_PRESENT_MODE_FIFO_RELAXED_KHR: return "VK_PRESENT_MODE_FIFO_RELAXED";
		default: return "VK_PRESENT_MODE_FIFO_RELAXED"; return buf;
        }
}

#define CASE_STR(x) case (x): return #x

const char *vk_format_string( VkFormat format )
{
	static char buf[16];
        
	switch ( format ) {
		// color formats
		CASE_STR( VK_FORMAT_R5G5B5A1_UNORM_PACK16 );
		CASE_STR( VK_FORMAT_B5G5R5A1_UNORM_PACK16 );
		CASE_STR( VK_FORMAT_R5G6B5_UNORM_PACK16 );
		CASE_STR( VK_FORMAT_B5G6R5_UNORM_PACK16 );
		CASE_STR( VK_FORMAT_B8G8R8A8_SRGB );
		CASE_STR( VK_FORMAT_R8G8B8A8_SRGB );
		CASE_STR( VK_FORMAT_B8G8R8A8_SNORM );
		CASE_STR( VK_FORMAT_R8G8B8A8_SNORM );
		CASE_STR( VK_FORMAT_B8G8R8A8_UNORM );
		CASE_STR( VK_FORMAT_R8G8B8A8_UNORM );
		CASE_STR( VK_FORMAT_B4G4R4A4_UNORM_PACK16 );
		CASE_STR( VK_FORMAT_R4G4B4A4_UNORM_PACK16 );
		CASE_STR( VK_FORMAT_R16G16B16A16_UNORM );
		CASE_STR( VK_FORMAT_A2B10G10R10_UNORM_PACK32 );
		CASE_STR( VK_FORMAT_A2R10G10B10_UNORM_PACK32 );
		CASE_STR( VK_FORMAT_B10G11R11_UFLOAT_PACK32 );
		// depth formats
		CASE_STR( VK_FORMAT_D16_UNORM );
		CASE_STR( VK_FORMAT_D16_UNORM_S8_UINT );
		CASE_STR( VK_FORMAT_X8_D24_UNORM_PACK32 );
		CASE_STR( VK_FORMAT_D24_UNORM_S8_UINT );
		CASE_STR( VK_FORMAT_D32_SFLOAT );
		CASE_STR( VK_FORMAT_D32_SFLOAT_S8_UINT );
                default:
		//sprintf( buf, sizeof( buf ), "#%i", format );
		return buf;
	}
}
#undef CASE_STR

PINLINE bool create_swapchain(vulkan_types* context, platform_state* plat_context)
{
        //choose a swap surface format
        uint32_t formatCount = 0;
        VkSurfaceFormatKHR surfaceFormats[20];
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->device_context.gpu, context->surface_context.surface, &formatCount, 0));
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->device_context.gpu, context->surface_context.surface, &formatCount, surfaceFormats));
        
        for(u32 i = 0; i < formatCount; i++)
        {
                VkSurfaceFormatKHR format = surfaceFormats[i];
                if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR &&
                    format.format == VK_FORMAT_B8G8R8A8_UNORM)
		{
			context->swapchain_context.swap_format.colorSpace = format.colorSpace;
			context->swapchain_context.swap_format.format = format.format;
                        break;
			
		}else{
                        context->swapchain_context.swap_format =format;
                        break;
                        
                }
        }
        
        printf("Using  format: %s for swapchain\n", vk_format_string(context->swapchain_context.swap_format.format));
        
        //choose a present mode
        u32 presentModesCount;
        VkPresentModeKHR  presentModes[100];
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context->device_context.gpu, context->surface_context.surface, &presentModesCount, NULL));
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context->device_context.gpu, context->surface_context.surface, &presentModesCount, presentModes));
        for(u32 i = 0;  i < presentModesCount; i++){
                printf("Present modes supported: %s\n", pmode_to_str(presentModes[i]));
        }
        
        VkPresentModeKHR desiredMode = VK_PRESENT_MODE_FIFO_KHR;
        for (uint32_t i = 0; i < presentModesCount; ++i)
	{
		// mode supported, nothing to do here
		if (presentModes[i] == desiredMode)
		{
                        
			
                        
			context->swapchain_context.swap_present_mode = desiredMode;
		}
	}
        
	VkPresentModeKHR usedPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	// preferred present mode not found - choose the next best thing
	for (uint32_t i = 0; i < presentModesCount; ++i)
	{
		// always prefer mailbox for triple buffering
		if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			usedPresentMode = presentModes[i];
			break;
		}
		else if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			usedPresentMode = presentModes[i];
                        context->swapchain_context.swap_present_mode = usedPresentMode;
		}
	}
        
        printf("Using present mode: %s\n", pmode_to_str(context->swapchain_context.swap_present_mode) );
        
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->device_context.gpu, context->surface_context.surface, &context->surface_context.surface_caps));
        VkExtent2D image_extent = context->surface_context.surface_caps.currentExtent;
        if ( image_extent.width == 0xffffffff && image_extent.height == 0xffffffff ) {
		image_extent.width = MIN( context->surface_context.surface_caps.maxImageExtent.width, MAX(context->surface_context. surface_caps.minImageExtent.width, (uint32_t)plat_context->win_width ) );
		image_extent.height = MIN( context->surface_context.surface_caps.maxImageExtent.height, MAX( context->surface_context.surface_caps.minImageExtent.height, (uint32_t) plat_context->win_height ) );
                
	}
        
        context->swapchain_context.swapchain_extent = image_extent;
        
        VkSwapchainCreateInfoKHR swapchain_info = {0};
        swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_info.pNext = NULL;
        swapchain_info.flags = 0;
        swapchain_info.surface = context->surface_context.surface;
        swapchain_info.minImageCount = 3;//imageCount;
        swapchain_info.imageFormat = context->swapchain_context.swap_format.format;
        swapchain_info.imageColorSpace = context->swapchain_context.swap_format.colorSpace;
        swapchain_info.imageExtent = context->swapchain_context.swapchain_extent;
        swapchain_info.imageArrayLayers = 1;
        swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_info.queueFamilyIndexCount = 0;
        swapchain_info.pQueueFamilyIndices = 0;
        swapchain_info.preTransform = context->surface_context.surface_caps.currentTransform;
        swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_info.presentMode = context->swapchain_context.swap_present_mode;
        swapchain_info.clipped = VK_TRUE;
        swapchain_info.oldSwapchain = 0;
        VK_CHECK(vkCreateSwapchainKHR(context->device_context.logical_device, &swapchain_info, context->instance_context.allocator, &context->swapchain_context.swapchain));
        
        printf("Created Swapchain\n");
        return true;
        
}
