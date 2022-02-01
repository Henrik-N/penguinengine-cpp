#include "rendererBackendVulkan.h"

// vulkan.hpp include here
#include "vulkanTypes.h"

#include <string>
#include <format>
#include <iostream>


import logger;

namespace vulkan {

    // This field is just so in case I want to write a custom allocator in the future, I won't need to go around and change every function

    void vulkan::init(stringv appName) {

        // todo set VkContext as a variable
        VkContext::init(appName);

    }

    void deinit() {

    }

    void onWindowResize(u32 width, u32 height) {

    }
}

