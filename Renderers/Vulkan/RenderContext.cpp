//
//
//

#include "RenderContext.hpp"
#include "../../Engine/Application.hpp"
#include "../../Engine/ApplicationWindow.hpp"

#include <Windows.h>

#include <set>

VulkanRenderContext::VulkanRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs) : IRenderContext(x, y, w, h, fs) {
	physicalDevice = VK_NULL_HANDLE;
	logicalDevice = VK_NULL_HANDLE;
	graphicsQueue = VK_NULL_HANDLE;
	graphicsQIndx = 0;
}

VulkanRenderContext::VulkanRenderContext(const IRenderContext& obj) : IRenderContext(obj) {
    physicalDevice = VK_NULL_HANDLE;
	logicalDevice = VK_NULL_HANDLE;
	graphicsQueue = VK_NULL_HANDLE;
	graphicsQIndx = 0;
}

VulkanRenderContext::~VulkanRenderContext() {
	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(logicalDevice, imageView, nullptr);
	}
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
	vkDestroyDevice(logicalDevice, nullptr);
}

void* VulkanRenderContext::getSurface() {
	return nullptr;
}

void VulkanRenderContext::setSurface( void* s, void* i ) {
	VkSurfaceKHR surface = reinterpret_cast<VkSurfaceKHR>(s);
	VkInstance instance = reinterpret_cast<VkInstance>(i);
	initializeGraphicsDevice( surface, instance );
}

void VulkanRenderContext::initializeGraphicsDevice( const VkSurfaceKHR& surface, const VkInstance& instance) {
	physicalDevice = VK_NULL_HANDLE;

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount < 1)
		throw std::runtime_error("No GPUs with Vulkan support!");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	unsigned int rating{ 0 };
	for (const auto& device : devices) {
		SwapChainSupportDetails details;
		unsigned int score = rateDeviceCompatiblities(device, surface, details);
		if (score > rating) {
			physicalDevice = device;
			swapChainCaps = details;
			rating = score;
		}
	}

	if (rating == 0)
		throw std::runtime_error("No Vulkan compatible GPUs meeting minimum rendering requirements found.");

	createDeviceGraphicsQueue(physicalDevice, surface);
	createSwapChain( surface );
}

void VulkanRenderContext::createDeviceGraphicsQueue(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	unsigned int i{ 0 };
	bool hasGfxQ{ false };
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			// TODO: this needs cleanup. Technically, you may never have a single queue that will do both graphics and
			// presentation. May always work in practice but should handle the possibility of separate queues. 
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if( presentSupport )
				break;
		}
		++i;
	}

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = i;
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (vkCreateDevice(device, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create logical device!");
	}

	vkGetDeviceQueue(logicalDevice, i, 0, &graphicsQueue);
}

unsigned int VulkanRenderContext::rateDeviceCompatiblities( const VkPhysicalDevice& device, const VkSurfaceKHR& surface, SwapChainSupportDetails& details ) {
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	if (!checkDeviceExtensionSupport(device))
		return 0;

	if (!querySwapChain(device, surface, details)) // Must be after extension check.
		return 0;
	else {
		bool haveSurface{ false };
		bool havePresentMode{ false };
		bool supportsGeom{ false };
		for (const VkSurfaceFormatKHR& format : details.formats) {
			if ((format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_UNORM) || format.format == VK_FORMAT_UNDEFINED) {
				haveSurface = true;
				break;
			}
		}
		for (const VkPresentModeKHR& mode : details.presentModes) {
			if (mode == VK_PRESENT_MODE_FIFO_KHR) {
				havePresentMode = true;
				break;
			}
		}

		if (_width <= details.capabilities.maxImageExtent.width && _height <= details.capabilities.maxImageExtent.height &&
			_width >= details.capabilities.minImageExtent.width && _height >= details.capabilities.minImageExtent.height) {
			supportsGeom = true;
		}

		if (!haveSurface || !havePresentMode || !supportsGeom)
			return 0;
	}

	if (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		return 100;
	else if (VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		return 50;
	else
		return 0;
}

bool VulkanRenderContext::checkDeviceExtensionSupport(const VkPhysicalDevice& device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool VulkanRenderContext::querySwapChain(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, SwapChainSupportDetails& details) {
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if( formatCount != 0 ) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return !details.formats.empty() && !details.presentModes.empty();
}

void VulkanRenderContext::createSwapChain( const VkSurfaceKHR& surface ) {
	uint32_t imageCount = swapChainCaps.capabilities.minImageCount + 1;
	if( swapChainCaps.capabilities.maxImageCount > 0 && imageCount > swapChainCaps.capabilities.maxImageCount )
		imageCount = swapChainCaps.capabilities.maxImageCount;

	VkSurfaceFormatKHR surfaceFormat{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	VkPresentModeKHR presentMode{ VK_PRESENT_MODE_FIFO_KHR };
	VkExtent2D extent = {_width, _height};

	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.pNext = nullptr;
	swapchainCreateInfo.minImageCount = imageCount;
	swapchainCreateInfo.imageFormat = surfaceFormat.format;
	swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainCreateInfo.imageExtent = extent;
	swapchainCreateInfo.imageArrayLayers = 1; // FYI: this in involved in setting up a VR pipeline
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // TODO: see VK_IMAGE_USAGE_TRANSFER_DST_BIT for blit from texture to swapchain.
	// TODO: see comments in createDeviceGraphicsQueue. These need to change when that does.
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCreateInfo.queueFamilyIndexCount = 0;
	swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	////////////////////////////////////////////////////////////////////////////////////////
	swapchainCreateInfo.preTransform = swapChainCaps.capabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = presentMode;
	swapchainCreateInfo.clipped = VK_TRUE; // Fine unless we need readback from swapchain (can't imaine a case for this)
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE; // For re-creating swapchain (e.g. window resize)
	swapchainCreateInfo.flags = 0;

	if (vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

	// Image views
	swapChainImageViews.resize(swapChainImages.size());
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = surfaceFormat.format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

__declspec(dllexport)
	std::shared_ptr<IRenderContext> CreateRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs) {
		std::shared_ptr<IRenderContext> context = std::make_shared<VulkanRenderContext>(x, y, w, h, fs);
		return context;
	}

__declspec(dllexport)
	std::shared_ptr<IRenderContext> CloneRenderContext(const IRenderContext& obj) {
		std::shared_ptr<IRenderContext> context = std::make_shared<VulkanRenderContext>(obj);
		return context;
	}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID) {
	return TRUE;
}
