#ifndef ZERO_CLIENT_GRAPHICS_DETAILS_HPP
#define ZERO_CLIENT_GRAPHICS_DETAILS_HPP

#include "header.hpp"
#include "internals.hpp"

namespace zero::graphics {
	class Details {
	public:
		uint32_t imageCount;
		uint32_t threadCount;

		vk::Extent2D swapchainExtent;
		vk::SurfaceFormatKHR surfaceFormat;
		vk::PresentModeKHR presentMode;
		vk::SurfaceTransformFlagBitsKHR swapchainTransform;

		//uint32_t uniformAlignment;
		//uint32_t uniformQueueStride;
		//uint32_t uniformFrameStride;
		//uint32_t uniformSize;

		vk::Format imageFormat;
		//vk::Format depthStencilFormat;

		uint32_t mipLevels;
		float maxAnisotropy;
		vk::SampleCountFlagBits sampleCount;

		vk::PhysicalDeviceMemoryProperties memoryProperties;

		Details(Internals &internals);
	};
}

#endif //ZERO_CLIENT_GRAPHICS_DETAILS_HPP
