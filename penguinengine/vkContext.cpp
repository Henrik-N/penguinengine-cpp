#include "vulkanTypes.h"

#include "defines.h"
#include <vector>
#include <iostream>
#include <format>
#include <set>



import platform;

import logger;

/// ============ Vulkan instance =============

vk::Instance
initInstance(stringv appName) {

    const std::string applicationName{ appName };

    const vk::ApplicationInfo appInfo{
        .pApplicationName = applicationName.c_str(),
        .applicationVersion = 1,
        .pEngineName = "penguin engine",
        .engineVersion = 1,
        .apiVersion = VK_API_VERSION_1_2, // TODO 1_3
    };

    // required layers

    #if _DEBUG
    const std::array<const char*, 1> requiredValidationLayerNames{
        "VK_LAYER_KHRONOS_validation",
    };
    #else
    const std::array<const char*, 0> requiredExtensionNames{};
    #endif

    // required extensions

    #if _DEBUG
    const std::array<const char*, 3> requiredExtensionNames = {
        platform::vulkan::getRequiredExtensionName(),
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    #else
    const std::array<const char*, 2> requiredExtensionNames = {
        platform::getRequiredVulkanExtensionName(),
        VK_KHR_SURFACE_EXTENSION_NAME,
    };
    #endif

    // ensure required layers are available
    const std::vector<vk::LayerProperties> availableLayerProps = vk::enumerateInstanceLayerProperties();

    for (const char* requiredLayer : requiredValidationLayerNames) {
        bool found = false;

        for (const vk::LayerProperties& availableLayer : availableLayerProps) {
            if (std::string(requiredLayer) == availableLayer.layerName) {
                found = true;
                break;
            }
        }

        if (found) {
            TRACE("Using vulkan layer: {}", requiredLayer);
        } else {
            ERROR("Required vulkan layer: {} not available", requiredLayer);
        }
    }

    const vk::InstanceCreateInfo instanceCreateInfo{
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<u32>(requiredValidationLayerNames.size()),
        .ppEnabledLayerNames = requiredValidationLayerNames.data(),
        .enabledExtensionCount = static_cast<u32>(requiredExtensionNames.size()),
        .ppEnabledExtensionNames = requiredExtensionNames.data(),
    };

    return vk::createInstance(instanceCreateInfo);
}


/// ============ Vulkan debug messenger =============
// declaring function pointers for callback functions to call
PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessenger;
PFN_vkDestroyDebugUtilsMessengerEXT pfnDestroyDebugUtilsMessenger;

void
setCreateDebugUtilsFnPointer(vk::Instance instance) {
    pfnCreateDebugUtilsMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));

    if (pfnCreateDebugUtilsMessenger == nullptr) {
        FATAL("couldn't find function pointer to vkCreateDebugUtilsMessengerEXT\n");
    }
}

void
setDestroyDebugUtilsFnPointer(vk::Instance instance) {
    pfnDestroyDebugUtilsMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));

    if (pfnDestroyDebugUtilsMessenger == nullptr) {
        FATAL("couldn't find function pointer to vkDestroyDebugUtilsMessengerEXT\n");
    }
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pMessenger
) {
    return pfnCreateDebugUtilsMessenger(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT messenger,
    VkAllocationCallbacks const* pAllocator
) {
    return pfnDestroyDebugUtilsMessenger(instance, messenger, pAllocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL
debugMessageFunc(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
    void* pUserData
) {
    using str = std::string;

    const str severity = vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity));
    const str types = vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes));
    const str idName = pCallbackData->pMessageIdName;
    const str idNumber = pCallbackData->pMessageIdName;
    const str message = pCallbackData->pMessage;

    str queueLabels = "";
    if (pCallbackData->queueLabelCount > 0) {
        queueLabels = "\tQueue Labels:\n";

        for (u8 i = 0; i < pCallbackData->queueLabelCount; i++) {
            queueLabels += std::format("\t\t{}\n", pCallbackData->pQueueLabels[i].pLabelName);
        }
    }

    str cmdBufferLabels = "";
    if (pCallbackData->cmdBufLabelCount > 0) {
        cmdBufferLabels = "\tCommandBuffer Labels:\n";

        for (u8 i = 0; i < pCallbackData->cmdBufLabelCount; i++) {
            cmdBufferLabels += std::format("\t\t{}\n", pCallbackData->pCmdBufLabels[i].pLabelName);
        }
    }

    str objects = "";
    if (pCallbackData->objectCount > 0) {
        for (u8 i = 0; i < pCallbackData->objectCount; i++) {
            const str objectType = vk::to_string(static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType));
            const str objectHandle = std::to_string(pCallbackData->pObjects[i].objectHandle);

            str objectName = "";
            if (pCallbackData->pObjects[i].pObjectName) {
                objectName = pCallbackData->pObjects[i].pObjectName;
            }

            objects = std::format(
                "\tObject:\n\t\tobject type: {}\n\t\tobject name:",
                objectType,
                objectHandle,
                objectName
            );
        }
    }

    //const str completeMsg = std::format("VULKAN VALIDATION: [{}]", severity);
    const str completeMsg = std::format("VULKAN VALIDATION [{}]: types: {}, idname: {},\n\t message: {}\n\tQueue labels {}\nCmdBufferLabels: {}\nObjects:\n{}",
        severity, types, idName, message, queueLabels, cmdBufferLabels, objects
    );

    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: DEBUG(completeMsg); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: INFO(completeMsg); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: WARNING(completeMsg); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: ERROR(completeMsg); break;
        default:
            DEBUG(completeMsg);
    }

    return false;
}

vk::DebugUtilsMessengerEXT
initDebugMessenger(vk::Instance instance) {

    using SevFlag = vk::DebugUtilsMessageSeverityFlagBitsEXT;

    const vk::DebugUtilsMessageSeverityFlagsEXT severity(
        SevFlag::eError
        | SevFlag::eWarning
        //| SevFlag::eInfo 
        //| SevFlag::eVerbose
    );

    using MFlags = vk::DebugUtilsMessageTypeFlagBitsEXT;

    const vk::DebugUtilsMessageTypeFlagsEXT types(
        MFlags::eGeneral
        | MFlags::ePerformance
        | MFlags::eValidation
    );

    setCreateDebugUtilsFnPointer(instance);
    setDestroyDebugUtilsFnPointer(instance);

    const vk::DebugUtilsMessengerCreateInfoEXT createInfo{
        .messageSeverity = severity,
        .messageType = types,
        .pfnUserCallback = &debugMessageFunc,
    };

    return instance.createDebugUtilsMessengerEXT(createInfo);
}


/// ============ Vulkan surface =============

#ifdef PENGUIN_PLATFORM_WINDOWS
#include <windows.h>
#include <windowsx.h>
#include "vulkan/vulkan_win32.h"

VkSurfaceKHR
initSurface(vk::Instance instance) {
    const platform::InternalState internalState = platform::vulkan::readInternalState();

    const vk::Win32SurfaceCreateInfoKHR createInfo{
        .hinstance = internalState.instance,
        .hwnd = internalState.window,
    };

    return instance.createWin32SurfaceKHR(createInfo);
}

#endif


/// ============ Vulkan physical device =============

struct QueueFamilies {
    Option<u32> graphicsFamily{};
    Option<u32> presentFamily{};

    static QueueFamilies find(vk::PhysicalDevice pd, vk::SurfaceKHR surface) {
        const std::vector<vk::QueueFamilyProperties> queueFamilyProps = pd.getQueueFamilyProperties();

        QueueFamilies queueFamilies{};

        for (u32 queueIndex = 0; queueIndex < queueFamilyProps.size(); queueIndex++) {
            // find queue with graphics queue flag
            if (queueFamilyProps[queueIndex].queueFlags & vk::QueueFlagBits::eGraphics) {
                queueFamilies.graphicsFamily = queueIndex;
            }

            if (pd.getSurfaceSupportKHR(queueIndex, surface)) {
                queueFamilies.presentFamily = queueIndex;
            }
        }
        return queueFamilies;
    }
};


struct PhysicalDeviceSuitablityInfo {
    vk::PhysicalDeviceProperties props;
    vk::PhysicalDeviceFeatures features;
};

namespace vulkan {
    VkContext::SwapchainSupport VkContext::SwapchainSupport::query(vk::PhysicalDevice pd, vk::SurfaceKHR surface) {
        return VkContext::SwapchainSupport{
            .capabilites = pd.getSurfaceCapabilitiesKHR(surface),
            .surfaceFormats = pd.getSurfaceFormatsKHR(surface),
            .presentModes = pd.getSurfacePresentModesKHR(surface),
        };
    }

    bool VkContext::SwapchainSupport::isSupportComplete() OK {
        return !surfaceFormats.empty() && !presentModes.empty();
    }
}


bool
isPhysicalDeviceSuitable(vk::PhysicalDevice pd, vk::SurfaceKHR surface, QueueFamilies queueFamilies) OK {

    // TODO rate alternatives if no discrete GPU is available
    if (pd.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        return true; // device is not discrete graphics card // TODO allow alternatives but sort them
    }

    if (!queueFamilies.graphicsFamily.has_value() || !queueFamilies.presentFamily.has_value()) {
        return false; // doesn't have required queues
    }

    if (!vulkan::VkContext::SwapchainSupport::query(pd, surface).isSupportComplete()) {
        return false; // doesn't have swapchain support
    }

    // check supports required extensions
    const std::vector<vk::ExtensionProperties> pdExtensionProps = pd.enumerateDeviceExtensionProperties();

    const std::vector<std::string> requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    i32 matches = 0;
    for (i32 i = 0; i < pdExtensionProps.size(); i++) {
        if (pdExtensionProps[i].extensionName == requiredExtensions[i]) {
            matches++;
        }
    }

    if (matches != requiredExtensions.size()) {
        return false; // doesn't have required extensions
    }

    return true;
}



std::pair<vk::PhysicalDevice, QueueFamilies>
selectPhysicalDevice(vk::Instance instance, vk::SurfaceKHR surface) {
    const std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();

    Option<vk::PhysicalDevice> outPd;
    QueueFamilies outQueueFamilies;

    for (const auto& pd : physicalDevices) {
        const auto queueFamilies = QueueFamilies::find(pd, surface);

        if (isPhysicalDeviceSuitable(pd, surface, queueFamilies)) {
            outPd = pd;
            outQueueFamilies = queueFamilies;
            break;
        }
    }

    if (!outPd.has_value()) {
        FATAL("Failed to find a suitable physical device!");
    }
    INFO("using {}", outPd.value().getProperties().deviceName);

    return std::pair<vk::PhysicalDevice, QueueFamilies>{outPd.value(), outQueueFamilies};
}


/// ============ Vulkan logical device =============

vk::Device
createDevice(vk::PhysicalDevice pd, QueueFamilies queueFamilies, vk::Queue& graphicsQueue, vk::Queue& presentQueue) {
    // grab unique families
    const std::set families = { queueFamilies.graphicsFamily.value(), queueFamilies.presentFamily.value() };

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    const float queuePriority = 1.f; // same for both
    for (u32 familyIndex : families) {
        const vk::DeviceQueueCreateInfo createInfo{
            .queueFamilyIndex = familyIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,
        };
        queueCreateInfos.emplace_back(createInfo);
    }

    // device features to use (better check that they are available first)
    constexpr const vk::PhysicalDeviceFeatures features{};


    constexpr const char* enabledExtensionNames = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    const vk::DeviceCreateInfo deviceCreateInfo{
        .queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = 0, // legacy
        .ppEnabledLayerNames = nullptr, // legacy
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = &enabledExtensionNames,
        .pEnabledFeatures = &features,
    };

    const vk::Device device = pd.createDevice(deviceCreateInfo);

    graphicsQueue = device.getQueue(queueFamilies.graphicsFamily.value(), 0);
    presentQueue = device.getQueue(queueFamilies.graphicsFamily.value(), 0);

    return device;
}


namespace vulkan {
    VkContext VkContext::init(stringv appName) {
        const vk::Instance instance = initInstance(appName);

        const vk::DebugUtilsMessengerEXT debugMessenger = initDebugMessenger(instance);

        const vk::SurfaceKHR surface = initSurface(instance);

        const auto [physicalDevice, queueFamilies] = selectPhysicalDevice(instance, surface);

        vk::Queue graphicsQueue;
        vk::Queue presentQueue;
        const vk::Device device = createDevice(physicalDevice, queueFamilies, graphicsQueue, presentQueue);

        return VkContext{
            .instance = instance,
            .debugMessenger = debugMessenger,
            .surface = surface,
            .physicalDevice = physicalDevice,
            .device = device,
            .graphicsQueue = graphicsQueue,
            .presentQueue = presentQueue,
        };
    }

    void VkContext::deinit() {
        device.destroy();

        instance.destroySurfaceKHR(surface);

        instance.destroyDebugUtilsMessengerEXT(debugMessenger);

        instance.destroy();
    }
}

