#include "image.h"

#include "memory.h"
#include "buffer.h"
#include "queue.h"

extern VkDevice device;

void createImage(Image *image, uint32_t width, uint32_t height, uint32_t levels, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage) {
    image->extent.width = width;
    image->extent.height = height;
    image->extent.depth = 1;
    image->levels = levels;
    image->samples = samples;
    image->format = format;
    image->usage = usage;

    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = image->format,
        .extent = image->extent,
        .mipLevels = image->levels,
        .arrayLayers = 1,
        .samples = image->samples,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = image->usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    vkCreateImage(device, &imageInfo, NULL, &image->image);
    vkGetImageMemoryRequirements(device, image->image, &image->memoryRequirements);
}

void bindImageMemory(Image *image, Memory *memory) {
    image->memory = memory;

    image->memoryOffset = alignMemory(memory, image->memoryRequirements);

    vkBindImageMemory(device, image->image, memory->memory, image->memoryOffset);
}

void createImageView(Image *image, VkImageAspectFlags aspects) {
    image->aspects = aspects;

    VkImageViewCreateInfo imageViewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .image = image->image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = image->format,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = {
            .aspectMask = image->aspects,
            .baseMipLevel = 0,
            .levelCount = image->levels,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    vkCreateImageView(device, &imageViewInfo, NULL, &image->view);
}

void copyBufferToImage(Buffer *buffer, Image *image, VkDeviceSize bufferOffset) {
    VkCommandBuffer commandBuffer = beginSingleTransferCommand();

    VkBufferImageCopy copyInfo = {
        .bufferOffset = bufferOffset,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = { // Should image parameters be used?
            .aspectMask = image->aspects,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
        .imageOffset = {
            .x = 0,
            .y = 0,
            .z = 0
        },
        .imageExtent = image->extent
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer->buffer, image->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo);
    endSingleTransferCommand(commandBuffer);
}

void recordTransitionImageLayout(VkCommandBuffer *commandBuffer, Image *image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags sourceAccessFlags, VkAccessFlags destinationAccessFlags, VkPipelineStageFlags sourceStages, VkPipelineStageFlags destinationStages) {
    VkImageMemoryBarrier memoryBarrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = NULL,
        .srcAccessMask = sourceAccessFlags,
        .dstAccessMask = destinationAccessFlags,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image->image,
        .subresourceRange = {
            .aspectMask = image->aspects,
            .baseMipLevel = 0,
            .levelCount = image->levels,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    vkCmdPipelineBarrier(*commandBuffer, sourceStages, destinationStages, 0, 0, NULL, 0, NULL, 1, &memoryBarrier);
}

void transitionImageLayout(Image *image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags sourceAccessFlags, VkAccessFlags destinationAccessFlags, VkPipelineStageFlags sourceStages, VkPipelineStageFlags destinationStages) {
    VkCommandBuffer commandBuffer = beginSingleTransferCommand();
    recordTransitionImageLayout(&commandBuffer, image, oldLayout, newLayout, sourceAccessFlags, destinationAccessFlags, sourceStages, destinationStages);
    endSingleTransferCommand(commandBuffer);
}

void destroyImageView(Image *image) {
    vkDestroyImageView(device, image->view, NULL);
}

void destroyImage(Image *image) {
    vkDestroyImage(device, image->image, NULL);

    image->memory = NULL;
}
