#pragma once


PINLINE bool create_sync_objects(vulkan_types* context)
{
        VkSemaphoreCreateInfo semaphoreInfo = {0};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VK_CHECK(vkCreateSemaphore(context->device_context.logical_device, &semaphoreInfo, 0, &context->sync_context.aquireSemaphore));
        vkCreateSemaphore(context->device_context.logical_device, &semaphoreInfo, 0, &context->sync_context.submitSemaphore);
        VkFenceCreateInfo fence_info = {0};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VK_CHECK(vkCreateFence(context->device_context.logical_device, &fence_info, context->instance_context.allocator, &context->sync_context.fence));
        return true;
}
