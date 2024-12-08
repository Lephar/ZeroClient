#pragma once

#include "zero.h"

#include "image.h"

struct framebuffer {
    Image depthStencil;
    Image color;
    Image resolve;

    VkCommandBuffer renderCommandBuffer;
    VkCommandBuffer presentCommandBuffer;

    VkSemaphore acquireSemaphore;
    VkSemaphore drawSemaphore;
    VkSemaphore blitSemaphore;

    VkFence renderFence;
    VkFence presentFence;
} typedef Framebuffer;

struct framebufferSet {
    uint32_t framebufferImageCount;

    VkSampleCountFlags sampleCount;

    VkFormat depthStencilFormat;
    VkFormat colorFormat;

    Framebuffer *framebuffers;
} typedef FramebufferSet;

void createFramebufferSet();
void destroyFramebufferSet();
