#pragma once

#define VULKAN_HPP_NO_SETTERS
#define VULKAN_HPP_NO_SMART_HANDLE
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_NODISCARD_WARNINGS
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL_log.h>

#include <glm/glm.hpp>

#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <limits>
