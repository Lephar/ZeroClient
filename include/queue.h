#pragma once

#include "zero.h"

struct queue {
    uint32_t queueFamilyIndex;
    uint32_t queueIndex;
    VkQueue queue;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
} typedef Queue;

uint32_t chooseQueueFamily(VkQueueFlags requiredFlags);