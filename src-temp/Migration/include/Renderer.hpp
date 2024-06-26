#pragma once

#include "Zero.hpp"

class Renderer {
	Surface *surface;
	Device *device;

};


/*
#ifndef NDEBUG
	VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
#endif // NDEBUG

struct Renderer {
	Window *window;

	PFN_vkGetInstanceProcAddr loader;

	vk::Instance instance;
#ifndef NDEBUG
	vk::DebugUtilsMessengerEXT messenger;
#endif // NDEBUG
	vk::SurfaceKHR surface;

	DeviceInfo deviceInfo;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
	vk::Queue queue;
	vk::CommandPool commandPool;

	Memory hostMemory;
	Memory deviceMemory;
	Memory imageMemory;

	Buffer uniformBuffer;
	Buffer stagingBuffer;
	Buffer elementBuffer;

	uint32_t swapchainSize;
	vk::SwapchainKHR swapchain;
	std::vector<vk::Image> swapchainImageHandles;

	vk::SampleCountFlagBits sampleCount;
	vk::Format depthStencilFormat;
	vk::Format colorFormat;
	uint32_t mipCount;
	std::vector<Framebuffer> framebuffers;

	void createInstance();
	void createSurface();

	vk::PhysicalDevice pickPhysicalDevice();
	uint32_t selectQueueFamily();
	void createDevice();

	vk::SurfaceFormatKHR selectSurfaceFormat();
	vk::PresentModeKHR selectPresentMode();

	void createSwapchain();
	void createFramebuffers();

	void createBuffers();
};
*/