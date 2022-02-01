module;


//#define VULKAN_HPP_NO_CONSTRUCTORS
//#include <vulkan/vulkan.hpp> // default vulkan header includes the keyword "module" which makes it invalid in C++20


module vulkan;


// vk::Instance createInst() {
// 	const vk::ApplicationInfo app_info{
// 		.pApplicationName = "sandbox",
// 		.applicationVersion = 1,
// 		.pEngineName = "penguin engine",
// 		.engineVersion = 1,
// 		.apiVersion = VK_API_VERSION_1_2, // TODO 1_3
// 	};
// 
// 	const vk::InstanceCreateInfo instance_create_info{
// 		.pApplicationInfo = &app_info,
// 	};
// 	
// 	auto props = vk::enumerateInstanceExtensionProperties();
// 	const vk::Instance instance = vk::createInstance(instance_create_info);
// 
// 	// return instance; // causes linker error
// 	// instance.destroy();
// 	// return vk::createInstance(instance_create_info, pvkalloc); // , nullptr, vk::getDispatchLoaderStatic());
// 
// 	return instance;
// }



