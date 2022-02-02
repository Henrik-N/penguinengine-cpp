#include "rendererBackend.h"


#ifdef PENGUIN_RENDERER_VULKAN
#include "rendererBackendVulkan.h"
#endif


void RendererBackend::init(stringv appName) {

    #ifdef PENGUIN_RENDERER_VULKAN

    vulkan::init(appName);


    vulkan::deinit();

    #endif

}

void RendererBackend::deinit() {

    #ifdef PENGUIN_RENDERER_VULKAN


    #endif

}

void RendererBackend::onWindowResize(u32 width, u32 height) {

    #ifdef PENGUIN_RENDERER_VULKAN

    vulkan::onWindowResize(width, height);

    #endif

}

