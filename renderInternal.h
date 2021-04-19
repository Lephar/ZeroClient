#ifndef ZERO_CLIENT_RENDER_HELPER_H
#define ZERO_CLIENT_RENDER_HELPER_H

struct svhDevice {
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkQueue queue;
    VkCommandPool commandPool;
} typedef SvhDevice;

struct svhBuffer {
    VkBuffer buffer;
    VkDeviceMemory memory;
} typedef SvhBuffer;

struct svhImage {
    VkImage image;
    VkImageView view;
    VkDeviceMemory memory;
} typedef SvhImage;

//TODO: Implement a better device selection
VkPhysicalDevice pickPhysicalDevice(VkInstance instance) {
    uint32_t physicalDeviceCount = 0u;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
    VkPhysicalDevice physicalDevices[physicalDeviceCount];
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices);

    for (uint32_t index = 0u; index <= physicalDeviceCount; index++) {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevices[index], &physicalDeviceProperties);

        if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            return physicalDevices[index];
    }

    return physicalDevices[0];
}

//TODO: Add exclusive queue family support
uint32_t selectQueueFamily(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice) {
    uint32_t queueFamilyPropertyCount = 0u;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, NULL);
    VkQueueFamilyProperties queueFamilyProperties[queueFamilyPropertyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties);

    for (uint32_t index = 0u; index < queueFamilyPropertyCount; index++) {
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, surface, &presentSupport);
        VkQueueFlags graphicsSupport = queueFamilyProperties[index].queueFlags & VK_QUEUE_GRAPHICS_BIT;

        if (presentSupport && graphicsSupport)
            return index;
    }

    return UINT32_MAX;
}

void createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags flags, uint32_t mipLevels,
                     VkImageView *view) {
    VkImageViewCreateInfo viewInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .image = image,
            .format = format,
            .components = {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = {
                    .aspectMask = flags,
                    .levelCount = mipLevels,
                    .baseMipLevel = 0,
                    .layerCount = 1,
                    .baseArrayLayer = 0
            }
    };

    vkCreateImageView(device, &viewInfo, NULL, view);
}

uint32_t chooseMemoryType(VkPhysicalDevice physicalDevice, uint32_t filter, VkMemoryPropertyFlags flags) {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (uint32_t index = 0u; index < memoryProperties.memoryTypeCount; index++)
        if ((filter & (1 << index)) && (memoryProperties.memoryTypes[index].propertyFlags & flags) == flags)
            return index;

    return UINT32_MAX;
}

void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory) {
    VkBufferCreateInfo bufferInfo = {
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .usage = usage,
            .size = size
    };

    vkCreateBuffer(device, &bufferInfo, NULL, buffer);

    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(device, *buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {
            .allocationSize = memoryRequirements.size,
            .memoryTypeIndex = chooseMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, properties)
    };

    vkAllocateMemory(device, &allocateInfo, NULL, bufferMemory);
    vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
}

void createImage(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t imageWidth, uint32_t imageHeight,
                 uint32_t mipLevels, VkSampleCountFlagBits samples, VkFormat format, VkImageTiling tiling,
                 VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image,
                 VkDeviceMemory *imageMemory) {
    VkImageCreateInfo imageInfo = {
            .imageType = VK_IMAGE_TYPE_2D,
            .extent.width = imageWidth,
            .extent.height = imageHeight,
            .extent.depth = 1,
            .mipLevels = mipLevels,
            .arrayLayers = 1,
            .format = format,
            .tiling = tiling,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .usage = usage,
            .samples = samples,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    vkCreateImage(device, &imageInfo, NULL, image);

    VkMemoryRequirements memoryRequirements = {};
    vkGetImageMemoryRequirements(device, *image, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {
            .allocationSize = memoryRequirements.size,
            .memoryTypeIndex = chooseMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, properties)
    };

    vkAllocateMemory(device, &allocateInfo, NULL, imageMemory);
    vkBindImageMemory(device, *image, *imageMemory, 0);
}

VkCommandBuffer beginSingleTimeCommand(VkDevice device, VkCommandPool commandPool) {
    VkCommandBufferAllocateInfo allocateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandPool = commandPool,
            .commandBufferCount = 1
    };

    VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocateInfo, &commandBuffer);
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void endSingleTimeCommand(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer
    };

    vkQueueSubmit(queue, 1, &submitInfo, NULL);
    vkQueueWaitIdle(queue);
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void copyBuffer(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer,
                VkDeviceSize size) {
    VkBufferCopy copyRegion = {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = size
    };

    VkCommandBuffer commandBuffer = beginSingleTimeCommand(device, commandPool);
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    endSingleTimeCommand(device, queue, commandPool, commandBuffer);
}

void copyBufferToImage(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkBuffer buffer, VkImage image,
                       uint32_t imageWidth, uint32_t imageHeight) {
    VkBufferImageCopy copyInfo = {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1
            },
            .imageOffset = {
                    .x = 0,
                    .y = 0,
                    .z = 0
            },
            .imageExtent = {
                    .width = imageWidth,
                    .height = imageHeight,
                    .depth = 1
            }
    };

    VkCommandBuffer commandBuffer = beginSingleTimeCommand(device, commandPool);
    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo);
    endSingleTimeCommand(device, queue, commandPool, commandBuffer);
}

void transitionImageLayout(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImage image,
                           VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) {
    VkAccessFlags sourceAccess = 0, destinationAccess = 0;
    VkPipelineStageFlags sourceStage = 0, destinationStage = 0;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        sourceAccess = 0;
        destinationAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        sourceAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
        destinationAccess = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    VkImageMemoryBarrier barrier = {
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcAccessMask = sourceAccess,
            .dstAccessMask = destinationAccess,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = mipLevels,
                    .baseArrayLayer = 0,
                    .layerCount = 1
            }
    };

    VkCommandBuffer commandBuffer = beginSingleTimeCommand(device, commandPool);
    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);
    endSingleTimeCommand(device, queue, commandPool, commandBuffer);
}

#endif
