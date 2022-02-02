#include "defines.h"
#include "vulkanTypes.h"

import logger;

const struct SurfaceFormat {
    static constexpr bool isSRGB(vk::SurfaceFormatKHR format) {
        return format.format == vk::Format::eB8G8R8A8Srgb  // a common srgb color format
            && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear; // linear color space
    }
};


vk::SurfaceFormatKHR 
selectSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& supportedFormats) {
    for (const auto& format : supportedFormats) {
        if (SurfaceFormat::isSRGB(format)) {
            INFO("Using SRGB color format with linear color space");
            return format;
        }
    }   
    INFO("not using SRGB");
    return supportedFormats.front();
}


namespace vulkan {
    void Swapchain::init(const VkContext& context, vk::Extent2D extent) {
        const auto swapchainSupport = context.querySwapchainSupport();

        const auto surfaceFormat = selectSwapSurfaceFormat(swapchainSupport.surfaceFormats);





    }

    void Swapchain::deinit() {

    }
}

