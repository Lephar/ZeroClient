#include "framebuffer.h"

#include "helper.h"
#include "queue.h"
#include "memory.h"
#include "swapchain.h"

extern VkExtent2D extent;
extern VkDevice device;
extern Queue graphicsQueue;
extern Memory deviceMemory;
extern Swapchain swapchain;

FramebufferSet framebufferSet;
FramebufferSet oldFramebufferSet;

void createFramebuffer(Framebuffer *framebuffer) {
    createImage(&framebuffer->depthStencil, extent.width, extent.height, 1, framebufferSet.sampleCount, framebufferSet.depthStencilFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    createImage(&framebuffer->color, extent.width, extent.height, 1, framebufferSet.sampleCount, framebufferSet.colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    createImage(&framebuffer->resolve, extent.width, extent.height, 1, VK_SAMPLE_COUNT_1_BIT, framebufferSet.colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);

    bindImageMemory(&framebuffer->depthStencil, &deviceMemory);
    bindImageMemory(&framebuffer->color, &deviceMemory);
    bindImageMemory(&framebuffer->resolve, &deviceMemory);

    createImageView(&framebuffer->depthStencil, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
    createImageView(&framebuffer->color, VK_IMAGE_ASPECT_COLOR_BIT);
    createImageView(&framebuffer->resolve, VK_IMAGE_ASPECT_COLOR_BIT);

    //transitionImageLayout(&framebuffer->depthStencil, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    //transitionImageLayout(&framebuffer->color, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    //transitionImageLayout(&framebuffer->resolve, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    framebuffer->renderCommandBuffer = allocateSingleCommandBuffer(&graphicsQueue);
    framebuffer->presentCommandBuffer = allocateSingleCommandBuffer(&graphicsQueue);

    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0
    };

    vkCreateSemaphore(device, &semaphoreInfo, NULL, &framebuffer->acquireSemaphore);
    vkCreateSemaphore(device, &semaphoreInfo, NULL, &framebuffer->drawSemaphore);
    vkCreateSemaphore(device, &semaphoreInfo, NULL, &framebuffer->blitSemaphoreRender);
    vkCreateSemaphore(device, &semaphoreInfo, NULL, &framebuffer->blitSemaphorePresent);

    // TODO: This is not an elegant solution, change to timeline semaphores perhaps?
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = NULL,
        .pWaitDstStageMask = NULL,
        .commandBufferCount = 0,
        .pCommandBuffers = NULL,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &framebuffer->blitSemaphoreRender
    };

    vkQueueSubmit(graphicsQueue.queue, 1, &submitInfo, VK_NULL_HANDLE);

    /*
    VkSemaphoreSignalInfo signalInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO,
        .pNext = NULL,
        .semaphore = framebuffer->blitSemaphore2,
        .value = 1
    };

    vkSignalSemaphore(device, &signalInfo);
    */

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    vkCreateFence(device, &fenceInfo, NULL, &framebuffer->drawFence);
    vkCreateFence(device, &fenceInfo, NULL, &framebuffer->blitFence);
}

void createFramebufferSet() {
    framebufferSet.framebufferImageCount = 2;

    framebufferSet.sampleCount = VK_SAMPLE_COUNT_4_BIT;

    framebufferSet.depthStencilFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
    framebufferSet.colorFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

    framebufferSet.framebuffers = malloc(framebufferSet.framebufferImageCount * sizeof(Framebuffer));

    for(uint32_t framebufferIndex = 0; framebufferIndex < framebufferSet.framebufferImageCount; framebufferIndex++) {
        createFramebuffer(&framebufferSet.framebuffers[framebufferIndex]);
        debug("Framebuffer %d created", framebufferIndex);
    }
}

void destroyFramebuffer(Framebuffer *framebuffer) {
    vkDestroyFence(device, framebuffer->drawFence, NULL);
    vkDestroyFence(device, framebuffer->blitFence, NULL);

    vkDestroySemaphore(device, framebuffer->acquireSemaphore, NULL);
    vkDestroySemaphore(device, framebuffer->drawSemaphore, NULL);
    vkDestroySemaphore(device, framebuffer->blitSemaphoreRender, NULL);
    vkDestroySemaphore(device, framebuffer->blitSemaphorePresent, NULL);

    destroyImageView(&framebuffer->resolve);
    destroyImageView(&framebuffer->color);
    destroyImageView(&framebuffer->depthStencil);

    destroyImage(&framebuffer->resolve);
    destroyImage(&framebuffer->color);
    destroyImage(&framebuffer->depthStencil);
}

void destroyFramebufferSet() {
    for(uint32_t framebufferIndex = 0; framebufferIndex < framebufferSet.framebufferImageCount; framebufferIndex++) {
        destroyFramebuffer(&framebufferSet.framebuffers[framebufferIndex]);
        debug("Framebuffer %d destroyed", framebufferIndex);
    }

    free(framebufferSet.framebuffers);
}
