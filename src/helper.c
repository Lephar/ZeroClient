#include "helper.h"

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback( VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                VkDebugUtilsMessageTypeFlagsEXT type,
                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                void* pUserData) {
    (void)severity;
    (void)type;
    (void)pUserData;

    debug("%s", pCallbackData->pMessage);

    return VK_FALSE;
}

// Not the most optimal solution but gets the job done
uint32_t popcount(uint32_t value) {
    uint32_t count = 0;

    while(value) {
        count += value & 1;
        value >>= 1;
    }

    return count;
}

uint32_t byte_to_binary(uint8_t value) {
    return (value == 0 || value == 1 ? value : ((value % 2) + 10 * byte_to_binary(value / 2)));
}