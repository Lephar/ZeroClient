#include "Graphics/Graphics.hpp"

#include <vulkan/vulkan_raii.hpp>

#include "Graphics/Instance.hpp"
#include "Graphics/Device.hpp"

namespace Graphics {
    Instance *instance;
    std::vector<Device *> devices;

    Instance *initialize(const char *title, std::vector<const char *> layers, std::vector<const char *> extensions) {
        instance = new Instance(title, layers, extensions);
/*
        vk::raii::PhysicalDevices physicalDevices{*instance};

        for(auto physicalDevice : physicalDevices) {
            auto device = new Device{physicalDevice};
            devices.push_back(device);
        }
*/
        return nullptr;
    }

    Instance *getInstance() {
        return instance;
    }

    Device *getDevice(size_t index) {
        return devices.at(index);
    }

    Device *getDefaultDevice() {
        return devices.front();
    }

    void destroy() {
        delete instance;
    }
}
