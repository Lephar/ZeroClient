#include "Engine.hpp"

namespace Engine {
	void run(const char* title, unsigned width, unsigned height) {
		System::initialize(title, width, height);
		Manager::initialize();
		Manager::loadFile("Lantern.gltf");

		auto loader = System::getLoader();
		auto extensions = System::getExtensions();

		auto instance = Graphics::createInstance(title, loader, extensions);
		auto surface = System::createSurface(instance);

		Graphics::registerSurface(surface);

		Graphics::createDevice();
		Graphics::createObjects();

		Graphics::createSwapchain(width, height);
		Graphics::createPipeline();

		//System::draw();

		Graphics::destroyPipeline();
		Graphics::destroySwapchain();
		Graphics::destroyObjects();
		Graphics::destroyDevice();
		Graphics::destroyCore();

		System::terminate();
	}
}
