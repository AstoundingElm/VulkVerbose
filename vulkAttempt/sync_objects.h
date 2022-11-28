#pragma once


PINLINE bool create_sync_objects(vulkan_types* context)
{
        VkSemaphoreCreateInfo semaphoreInfo = {0};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VK_CHECK(vkCreateSemaphore(context->logical_device, &semaphoreInfo, 0, &context->aquireSemaphore));
        vkCreateSemaphore(context->logical_device, &semaphoreInfo, 0, &context->submitSemaphore);
        VkFenceCreateInfo fence_info = {0};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VK_CHECK(vkCreateFence(context->logical_device, &fence_info, context->allocator, &context->fence));
        return true;
}
