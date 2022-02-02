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
        struct SwapchainSupport {
            vk::SurfaceCapabilitiesKHR capabilites;
            std::vector<vk::SurfaceFormatKHR> surfaceFormats;
            std::vector<vk::PresentModeKHR> presentModes;

            static SwapchainSupport
                query(vk::PhysicalDevice pd, vk::SurfaceKHR surface);
            bool
                isSupportComplete() OK;
        };

        vk::Instance instance;
        vk::DebugUtilsMessengerEXT debugMessenger;
        vk::SurfaceKHR surface;

        vk::PhysicalDevice physicalDevice;

        vk::Device device;
        vk::Queue graphicsQueue;
        vk::Queue presentQueue;

        static VkContext
            init(stringv appName);
        void
            deinit();

        SwapchainSupport 
            querySwapchainSupport() const { return SwapchainSupport::query(physicalDevice, surface); }
        vk::PhysicalDeviceMemoryProperties
            getPhysicalDeviceMemoryProperties() const OK { return physicalDevice.getMemoryProperties(); }
    };


    struct Swapchain {
        const VkContext* context;
        vk::SurfaceFormatKHR surfaceFormat; // aka color depth
        //vk::PresentModeKHR presentMode;
        vk::Extent2D extent;
        vk::SwapchainKHR swapchain;


        static void
            init(const VkContext& context, vk::Extent2D extent);
        void
            deinit();
    };
}

