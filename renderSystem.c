#include "renderSystem.h"
#include "renderHelper.h"

#include <stdio.h>
#include <string.h>

VkInstance instance;
VkSurfaceKHR surface;
VkPhysicalDevice physicalDevice;
uint32_t queueFamilyIndex;
VkDevice device;
VkQueue queue;
VkCommandPool commandPool;

VkDebugUtilsMessengerEXT messenger;

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
											   VkDebugUtilsMessageTypeFlagsEXT type,
											   const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
											   void *pUserData) {
	(void) type;
	(void) pUserData;

	if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		printf("%s\n", pCallbackData->pMessage);

	return VK_FALSE;
}

void createInstance(uint32_t contextExtensionCount, const char **contextExtensionNames, VkInstance *instanceHandle) {
	uint32_t layerCount = 1u, extensionCount = contextExtensionCount + 1;
	const char *layerNames[] = {"VK_LAYER_KHRONOS_validation"}, *extensionNames[extensionCount];
	memcpy(extensionNames, contextExtensionNames, (extensionCount - 1) * sizeof(const char *));
	extensionNames[extensionCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

	VkApplicationInfo applicationInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = "Zero",
			.applicationVersion = VK_MAKE_VERSION(0, 1, 0),
			.pEngineName = "Zero Engine",
			.engineVersion = VK_MAKE_VERSION(0, 1, 0),
			.apiVersion = VK_API_VERSION_1_2
	};

	VkDebugUtilsMessengerCreateInfoEXT messengerInfo = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
							   VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
							   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
							   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = messageCallback
	};

	VkInstanceCreateInfo instanceInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &applicationInfo,
			.enabledLayerCount = layerCount,
			.ppEnabledLayerNames = layerNames,
			.enabledExtensionCount = extensionCount,
			.ppEnabledExtensionNames = extensionNames,
			.pNext = &messengerInfo
	};

	vkCreateInstance(&instanceInfo, NULL, &instance);
	PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)
			vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	createDebugUtilsMessenger(instance, &messengerInfo, NULL, &messenger);
	*instanceHandle = instance;
}

void bindSurface(VkSurfaceKHR surfaceHandle) {
	surface = surfaceHandle;
}

void createDevice() {
    float queuePriority = 1.0f;
    VkPhysicalDeviceFeatures deviceFeatures = {};

    uint32_t extensionCount = 1u;
    const char *extensionNames[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkDeviceQueueCreateInfo queueInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queueFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
    };

    VkDeviceCreateInfo deviceInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queueInfo,
            .pEnabledFeatures = &deviceFeatures,
            .enabledExtensionCount = extensionCount,
            .ppEnabledExtensionNames = extensionNames
    };

    VkCommandPoolCreateInfo poolInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = queueFamilyIndex
    };

    vkCreateDevice(physicalDevice, &deviceInfo, NULL, &device);
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);
    vkCreateCommandPool(device, &poolInfo, NULL, &commandPool);
}

void prepareRenderer(void) {
	physicalDevice = pickPhysicalDevice(instance);
	queueFamilyIndex = selectQueueFamily(surface, physicalDevice);
    createDevice();
}

void destroyRenderer(void) {
    vkDestroyCommandPool(device, commandPool, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);
	PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)
			vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	destroyDebugUtilsMessenger(instance, messenger, NULL);
	vkDestroyInstance(instance, NULL);
}