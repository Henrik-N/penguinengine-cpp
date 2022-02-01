#pragma once
#include "defines.h"

/// Vulkan stuff is currently hard to implement in a module because it uses the word "module"
///	in many place where it collides with the C++20 module keyword.
/// I will hopefully be able to move this to a module in the future.
/// 
/// https://github.com/KhronosGroup/Vulkan-Hpp/issues/121
/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0795r0.pdf

//#define VULKAN_HPP_NO_SPACESHIP_OPERATOR
#define VULKAN_HPP_NO_CONSTRUCTORS

#ifdef PENGUIN_PLATFORM_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.hpp> // default vulkan header includes the keyword "module" which makes it invalid in C++20


namespace vulkan {
    struct VkContext {
        const vk::Instance instance;
        const vk::DebugUtilsMessengerEXT debugMessenger;
        const vk::SurfaceKHR surface;

        const vk::PhysicalDevice physicalDevice;

        const vk::Device device;
        const vk::Queue graphicsQueue;
        const vk::Queue presentQueue;

        static VkContext
            init(stringv appName);

        void
            deinit();
    };


    struct Swapchain {
        const VkContext* context;
        vk::SurfaceFormatKHR surfaceFormat;
        // vk::PresentModeKHR presentMode;

    };
}

