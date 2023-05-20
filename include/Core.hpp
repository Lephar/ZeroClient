#pragma once

#define VULKAN_HPP_NO_SETTERS
#define VULKAN_HPP_NO_SMART_HANDLE
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_NODISCARD_WARNINGS
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vulkan/vulkan.hpp>

class Core {
private:
	std::string title;

	SDL_Window* window;
	vk::Extent2D extent;

	PFN_vkGetInstanceProcAddr loader;

	vk::Instance instance;
#ifndef NDEBUG
	vk::DebugUtilsMessengerEXT messenger;
#endif // NDEBUG
	vk::SurfaceKHR surface;

	vk::PhysicalDevice physicalDevice;
	vk::Device device;

	vk::Queue transferQueue;
	vk::Queue graphicsQueue;

	vk::SwapchainKHR swapchain;
	std::vector<vk::Image> swapchainImages;

	void createWindow();
	void createInstance();
	void createSurface();

	vk::PhysicalDevice pickPhysicalDevice();
	void selectQueueFamilies(unsigned int& transferQueueFamilyIndex, unsigned int& graphicsQueueFamilyIndex);
	void createDevice();

	void createSwapchain();

public:
	Core(const char* title, unsigned int width, unsigned int height);

	void draw(void (*render)(void));

	~Core();
};
