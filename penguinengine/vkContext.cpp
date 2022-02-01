#include "vulkanTypes.h"

#include "defines.h"
#include <vector>
#include <iostream>
#include <format>
#include <set>



import platform;



import logger;

// in case I want to implement it in the future
const vk::AllocationCallbacks* pAlloc = nullptr;

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

    return vk::createInstance(instanceCreateInfo, pAlloc);
}


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

    return instance.createDebugUtilsMessengerEXT(createInfo, pAlloc);
}


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


struct PhysicalDeviceSuitablityInfo {
    vk::PhysicalDeviceProperties props;
    vk::PhysicalDeviceFeatures features;
};


bool
isPhysicalDeviceSuitable(vk::PhysicalDevice physicalDevice, const PhysicalDeviceSuitablityInfo& info) OK {
    // TODO rate alternatives if no discrete GPU is available
    if (info.props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        return true;
    }
    return false;
}


struct QueueFamilies {
    Option<u32> graphicsFamily{};
    Option<u32> presentFamily{};
};




QueueFamilies
findQueueFamilies(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {

    const std::vector<vk::QueueFamilyProperties> queueFamilyProps = physicalDevice.getQueueFamilyProperties();

    QueueFamilies queueFamilies{};

    for (u32 queueIndex = 0; queueIndex < queueFamilyProps.size(); queueIndex++) {
        // find queue with graphics queue flag
        if (queueFamilyProps[queueIndex].queueFlags & vk::QueueFlagBits::eGraphics) {
            queueFamilies.graphicsFamily = queueIndex;
        }

        // try to find a present queue that isn't the same as the graphics queue
        if (physicalDevice.getSurfaceSupportKHR(queueIndex, surface)) {
            queueFamilies.presentFamily = queueIndex;
        }
    }

    if (!queueFamilies.graphicsFamily.has_value()) {
        FATAL("couldn't find a graphics queue family");
    } else if (physicalDevice.getSurfaceSupportKHR(queueFamilies.graphicsFamily.value(), surface)) {
        // if the graphics queue has present support, 
        // prefer this as it will probably be more performant using a single queue for this in most cases
        queueFamilies.presentFamily = queueFamilies.graphicsFamily.value();
    }
    if (!queueFamilies.presentFamily.has_value()) {
        FATAL("couldnt find a present queue family");
    }

    return queueFamilies;
}


std::pair<vk::PhysicalDevice, QueueFamilies>
selectPhysicalDevice(vk::Instance instance, vk::SurfaceKHR surface) {
    const std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();

    usize idx = -1;
    for (usize i = 0; i < physicalDevices.size(); i++) {
        if (physicalDevices[i].getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            idx = i;
            break;
        }
    }

    if (idx < 0) {
        FATAL("No discrete gpus found");
    } else {
        INFO("Using gpu: {}", physicalDevices[idx].getProperties().deviceName);
    }


    /// TODO suitablity checks and ensure the device has the required extensions and swapchain is supported

    const QueueFamilies queueFamilies = findQueueFamilies(physicalDevices[idx], surface);
    return std::make_pair(physicalDevices[idx], queueFamilies);
}


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
    VkContext
        VkContext::init(stringv appName) {

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
            .graphicsQueue = graphicsQueue,
            .presentQueue = presentQueue,
        };
    }

    void
        VkContext::deinit() {

        device.destroy();

        instance.destroySurfaceKHR(surface);

        instance.destroyDebugUtilsMessengerEXT(debugMessenger, pAlloc);

        instance.destroy(pAlloc);
    }
}

