#include "rendererBackendVulkan.h"

// vulkan.hpp include here
#include "vulkanTypes.h"

#include <string>
#include <format>
#include <iostream>


import logger;

namespace vulkan {
    struct State {
        VkContext context;
    };

    static State state{};

    // This field is just so in case I want to write a custom allocator in the future, I won't need to go around and change every function

    void vulkan::init(stringv appName) {

        // todo set VkContext as a variable
        VkContext context = VkContext::init(appName);

        Swapchain::init(context, vk::Extent2D(1, 1));


        state.context = context;
    }

    void deinit() {
        state.context.deinit();
    }

    void onWindowResize(u32 width, u32 height) {

    }
}

