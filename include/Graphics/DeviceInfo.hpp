#pragma once

#include "Graphics.hpp"

class DeviceInfo {
	friend class Renderer;

	vk::PhysicalDeviceProperties deviceProperties;
	vk::PhysicalDeviceFeatures deviceFeatures;
	vk::PhysicalDeviceMemoryProperties memoryProperties;
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
	vk::SurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<vk::SurfaceFormatKHR> surfaceFormats;
	std::vector<vk::PresentModeKHR> presentModes;
	std::vector<vk::Bool32> queueFamilySurfaceSupport;

    void initialize(vk::PhysicalDevice &physicalDevice, vk::SurfaceKHR &surface);
};