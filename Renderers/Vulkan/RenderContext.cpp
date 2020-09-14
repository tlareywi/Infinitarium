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
	currentSwapFrame = 0;
}

VulkanRenderContext::VulkanRenderContext(const IRenderContext& obj) : IRenderContext(obj) {
    physicalDevice = VK_NULL_HANDLE;
	logicalDevice = VK_NULL_HANDLE;
	graphicsQueue = VK_NULL_HANDLE;
	graphicsQIndx = 0;
	currentSwapFrame = 0;
}

VulkanRenderContext::~VulkanRenderContext() {
	vkDeviceWaitIdle(logicalDevice);
	 
	for( size_t i = 0; i < swapChainImages.size(); i++ ) {
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphore[i], nullptr);
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphore[i], nullptr);
		vkDestroyFence(logicalDevice, swapChainFences[i], nullptr);
	}

	for (auto imageView : swapChainImageViews)
		vkDestroyImageView(logicalDevice, imageView, nullptr);

	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
	vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
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
	createDescriptorPool();
}

void VulkanRenderContext::createDeviceGraphicsQueue(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	unsigned int graphicsQueueIndx{ 0 };
	bool hasGfxQ{ false };
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			// TODO: this needs cleanup. Technically, you may never have a single queue that will do both graphics and
			// presentation. May always work in practice but should handle the possibility of separate queues. 
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, graphicsQueueIndx, surface, &presentSupport);
			if( presentSupport )
				break;
		}
		++graphicsQueueIndx;
	}

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = graphicsQueueIndx;
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

	vkGetDeviceQueue(logicalDevice, graphicsQueueIndx, 0, &graphicsQueue);

	// Command Pool creation /////////////////////////////////////////////////////////////////////////
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphicsQueueIndx;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional
	if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void VulkanRenderContext::createDescriptorPool() {
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(swapChainImages.size());

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;

	poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

	if (vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
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

	assert( vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr) == VK_SUCCESS );
	swapChainImages.resize(imageCount);
	assert( vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data()) == VK_SUCCESS );

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

	size_t swapChainSize{ swapChainImages.size() };
	imageAvailableSemaphore.resize( swapChainSize );
	renderFinishedSemaphore.resize( swapChainSize );
	swapChainFences.resize( swapChainSize, VK_NULL_HANDLE );
	inFlightFences.resize( swapChainSize, VK_NULL_HANDLE);
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to create semaphores!");
		if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to create semaphores!");
		if( vkCreateFence(logicalDevice, &fenceInfo, nullptr, &swapChainFences[i]) != VK_SUCCESS )
			throw std::runtime_error("failed to create synchronization objects for a frame!");
	}
}

uint32_t VulkanRenderContext::nextSwapChainTarget() {
	uint32_t swapChainIndx;
	currentSwapFrame = currentSwapFrame % swapChainImages.size();

	if( vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphore[currentSwapFrame++], VK_NULL_HANDLE, &swapChainIndx) != VK_SUCCESS )
		throw std::runtime_error("vkAcquireNextImageKHR returned error code!");

	if (inFlightFences[swapChainIndx] != VK_NULL_HANDLE)
		assert( vkWaitForFences(logicalDevice, 1, &inFlightFences[swapChainIndx], VK_TRUE, UINT64_MAX) == VK_SUCCESS );

	inFlightFences[swapChainIndx] = swapChainFences[swapChainIndx];
	
	// This appears to always be true for now. If it's ever hit then I need to rethink the fence design a bit. 
	assert( swapChainIndx == (currentSwapFrame - 1) );

	return swapChainIndx;
}

void VulkanRenderContext::submit(VkSubmitInfo& submitInfo) {
	if( vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS )
		throw std::runtime_error("failed to submit command buffer!");

	// TODO: A fence would allow you to schedule multiple transfers simultaneouslyand wait for all of them complete, instead of executing one at a time.That may give the driver more opportunities to optimize.
	vkQueueWaitIdle(graphicsQueue);
}

void VulkanRenderContext::submit( VkCommandBuffer& bufffer, uint32_t frameIndx ) {
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore[frameIndx] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &bufffer;
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore[frameIndx] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(logicalDevice, 1, &swapChainFences[frameIndx]);

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, swapChainFences[frameIndx]) != VK_SUCCESS)
		throw std::runtime_error("failed to submit draw command buffer!");
}

void VulkanRenderContext::present( uint32_t frameIndx ) {
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore[frameIndx] };
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &frameIndx;

	presentInfo.pResults = nullptr; // Optional

	if( vkQueuePresentKHR(graphicsQueue, &presentInfo) != VK_SUCCESS )
		throw std::runtime_error("failed to present!");
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
