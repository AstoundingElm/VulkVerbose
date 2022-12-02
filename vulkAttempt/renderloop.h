#pragma once

#include "vulkan_types.h"


bool render_loop(vulkan_types* context)
{
        
        uint32_t imgIdx;
        
        // We wait on the GPU to be done with the work
        VK_CHECK(vkWaitForFences(context->device_context.logical_device, 1, &context->sync_context.fence, VK_TRUE, UINT64_MAX));
        VK_CHECK(vkResetFences(context->device_context.logical_device, 1, &context->fence));
        
        // Copy transforms to the buffer
        {
                vk_copy_to_buffer(&vkcontext->transformStorageBuffer, &gameState->entities, sizeof(Transform) * gameState->entityCount);
        }
        
        // Get Descriptor and add Render Command
        {
                Descriptor *desc = vk_get_descriptor(vkcontext, ASSET_SPRITE_CAKEZ);
                
                if (desc)
                {
                        RenderCommand *rc = vk_add_render_command(vkcontext, desc);
                        rc->instanceCount = gameState->entityCount;
                        rc->pushData.transformIdx = 0;
                }
        }
        
        // This waits on the timeout until the image is ready, if timeout reached -> VK_TIMEOUT
        VK_CHECK(vkAcquireNextImageKHR(vkcontext->device, vkcontext->swapchain, UINT64_MAX, vkcontext->aquireSemaphore, 0, &imgIdx));
        
        VkCommandBuffer cmd = vkcontext->cmd;
        vkResetCommandBuffer(cmd, 0);
        
        VkCommandBufferBeginInfo beginInfo = cmd_begin_info();
        VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));
        
        // Clear Color to Yellow
        VkClearValue clearValue = {};
        clearValue.color = {1, 1, 0, 1};
        
        VkRenderPassBeginInfo rpBeginInfo = {};
        rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rpBeginInfo.renderArea.extent = vkcontext->screenSize;
        rpBeginInfo.clearValueCount = 1;
        rpBeginInfo.pClearValues = &clearValue;
        rpBeginInfo.renderPass = vkcontext->renderPass;
        rpBeginInfo.framebuffer = vkcontext->framebuffers[imgIdx];
        vkCmdBeginRenderPass(cmd, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        
        VkViewport viewport = {};
        viewport.maxDepth = 1.0f;
        viewport.width = vkcontext->screenSize.width;
        viewport.height = vkcontext->screenSize.height;
        
        VkRect2D scissor = {};
        scissor.extent = vkcontext->screenSize;
        
        vkCmdSetViewport(cmd, 0, 1, &viewport);
        vkCmdSetScissor(cmd, 0, 1, &scissor);
        
        vkCmdBindIndexBuffer(cmd, vkcontext->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vkcontext->pipeline);
        
        // Render Loop
        {
                for (uint32_t i = 0; i < vkcontext->renderCommandCount; i++)
                {
                        RenderCommand *rc = &vkcontext->renderCommands[i];
                        
                        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vkcontext->pipeLayout,
                                                0, 1, &rc->desc->set, 0, 0);
                        
                        vkCmdPushConstants(cmd, vkcontext->pipeLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushData), &rc->pushData);
                        
                        vkCmdDrawIndexed(cmd, 6, rc->instanceCount, 0, 0, 0);
                }
                
                // Reset the Render Commands for next Frame
                vkcontext->renderCommandCount = 0;
        }
        
        vkCmdEndRenderPass(cmd);
        
        VK_CHECK(vkEndCommandBuffer(cmd));
        
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        
        // This call will signal the Fence when the GPU Work is done
        VkSubmitInfo submitInfo = submit_info(&cmd);
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.pSignalSemaphores = &vkcontext->submitSemaphore;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &vkcontext->aquireSemaphore;
        submitInfo.waitSemaphoreCount = 1;
        VK_CHECK(vkQueueSubmit(vkcontext->graphicsQueue, 1, &submitInfo, vkcontext->imgAvailableFence));
        
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pSwapchains = &vkcontext->swapchain;
        presentInfo.swapchainCount = 1;
        presentInfo.pImageIndices = &imgIdx;
        presentInfo.pWaitSemaphores = &vkcontext->submitSemaphore;
        presentInfo.waitSemaphoreCount = 1;
        vkQueuePresentKHR(vkcontext->graphicsQueue, &presentInfo);
        
        return true;
        
        
}