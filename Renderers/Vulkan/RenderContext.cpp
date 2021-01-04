//
//
//

#include "RenderContext.hpp"
#include "OpenXRContext.hpp"
#include "../../Engine/Application.hpp"
#include "../../Engine/ApplicationWindow.hpp"

#include <Windows.h>

#include <set>
#include <thread>

VulkanRenderContext::VulkanRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs, bool headset) : IRenderContext(x, y, w, h, fs, headset) {
	physicalDevice = VK_NULL_HANDLE;
	logicalDevice = VK_NULL_HANDLE;
	graphicsQueue = VK_NULL_HANDLE;
	targetInFlight = 0;
	renderingPaused = false;
	toggleFullscreen = false;
}

VulkanRenderContext::VulkanRenderContext(const IRenderContext& obj) : IRenderContext(obj) {
    physicalDevice = VK_NULL_HANDLE;
	logicalDevice = VK_NULL_HANDLE;
	graphicsQueue = VK_NULL_HANDLE;
	targetInFlight = 0;
	renderingPaused = false;
	toggleFullscreen = false;
}

VulkanRenderContext::~VulkanRenderContext() {
	if (!logicalDevice) return;
	 
	deAllocSwapchain();

	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
	vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
	vkDestroyDevice(logicalDevice, nullptr);
}

VkCommandBuffer VulkanRenderContext::allocTransientBuffer() {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	// TODO:  You may wish to create a separate command pool for these kinds of short-lived buffers, because the 
	// implementation may be able to apply memory allocation optimizations. You should use the VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
	// flag during command pool generation in that case.

	VkCommandBuffer commandBuffer;
	CheckVkResult(vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer));

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	return commandBuffer;
}

void VulkanRenderContext::attachTargets(IRenderPass& renderPass) {
	for (auto& target : swapchainTargets) {
		target.setExtent(swapchainCreateInfo.imageExtent.width, swapchainCreateInfo.imageExtent.height);
		target.attach(*this, renderPass);
	}
}

void VulkanRenderContext::beginFrame() {
	CheckVkResult(vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphore[targetInFlight], VK_NULL_HANDLE, &targetInFlight));
	newFrame = true;
}

void VulkanRenderContext::deAllocSwapchain() {
	for (size_t i = 0; i < swapchainTargets.size(); i++) {
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphore[i], nullptr);
	}

	swapchainTargets.clear(); // Must be done before device destruction

	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
}

VkDeviceCreateInfo& VulkanRenderContext::createDeviceQueueInfo(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			if (surface) {
				// Check for presentation support on the queue.
				// TODO: Technically the graphics and presentation queues may be separate, should extend this to support that configuration (not sure how common it is). 
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, graphicsQueueIndx, surface, &presentSupport);
				if (presentSupport)
					break;
			}
			else 
				break;
		}
		++graphicsQueueIndx;
	}

	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = graphicsQueueIndx;
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	return deviceCreateInfo;
}

void VulkanRenderContext::createDeviceQueue() {
	vkGetDeviceQueue(logicalDevice, graphicsQueueIndx, 0, &graphicsQueue);

	// Command Pool creation /////////////////////////////////////////////////////////////////////////
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphicsQueueIndx;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional
	if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::runtime_error("failed to create command pool!");
}

void VulkanRenderContext::createDeviceGraphicsQueue(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) {
	createDeviceQueueInfo(device, surface);

	if (vkCreateDevice(device, &deviceCreateInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create logical device!");
	}

	createDeviceQueue();
}

void VulkanRenderContext::createDescriptorPool() {
	std::vector<VkDescriptorPoolSize> poolSizes;
	
	{
		VkDescriptorPoolSize poolSize;
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = 1;
		poolSizes.push_back(poolSize);
	}

	{
		VkDescriptorPoolSize poolSize;
		poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSize.descriptorCount = 6; // Arbitrary, 6 buffers should be enough for anyone :P
		poolSizes.push_back(poolSize);
	}

	{
		VkDescriptorPoolSize poolSize;
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = 4; // Arbitrary
		poolSizes.push_back(poolSize);
	}

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();

	// Only 4 descriptor sets can be 'bound' simulatainiously on many platforms. Somthing to keep in mind. For max sets allocated, we'll choose a rather arbitary number for now. 
	poolInfo.maxSets = 16;

	if (vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
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
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
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

	std::vector<VkImage> swapChainImages;
	CheckVkResult( vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr));
	swapChainImages.resize(imageCount);
	CheckVkResult(vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data()));

	// Render targets (image + imageview + framebuffer)
	swapchainTargets.reserve(imageCount);
	for (size_t i = 0; i < imageCount; i++) {
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

		VulkanRenderTarget target( logicalDevice, createInfo );
		swapchainTargets.emplace_back(std::move(target));
	}

	size_t swapChainSize{ imageCount };
	imageAvailableSemaphore.resize( swapChainSize );
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to create semaphores!");
	}
}

void VulkanRenderContext::endFrame() {
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	VkSemaphore waitSemaphores[] = { renderFinishedSemaphores.back() };
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = waitSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &targetInFlight;

	presentInfo.pResults = nullptr; // Optional

	if (vkQueuePresentKHR(graphicsQueue, &presentInfo) != VK_SUCCESS) {
		while (renderingPaused) // Generally indecates app is minimized
			std::this_thread::yield();

		reAllocSwapchain();
	}

	renderFinishedSemaphores.clear();

	targetInFlight = (targetInFlight + 1) % swapchainTargets.size();

	if (toggleFullscreen) {
		toggleFullscreen = false;
		renderingPaused = true;
		window->toggleFullScreen(*this);
		while(renderingPaused) // The glfw framebuffer size callback will unpause us when the new framebuffer is up. 
			std::this_thread::yield();
		reAllocSwapchain();
	}
}

void* VulkanRenderContext::getSurface() {
	return nullptr;
}

VulkanRenderTarget& VulkanRenderContext::getSwapchainTarget() {
	return swapchainTargets[targetInFlight];
}

void VulkanRenderContext::initializeGraphicsDevice(const VkSurfaceKHR& surface, const VkInstance& instance) {
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

	recreateSwapchain = [this](VkSurfaceKHR& surface) {
		createSwapChain(surface);
	};

	createSwapChain(surface);
	createDescriptorPool();
}

void VulkanRenderContext::pauseRendering(bool pause) {
	renderingPaused = pause;
}

bool VulkanRenderContext::querySwapChain(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, SwapChainSupportDetails& details) {
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0) {
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

unsigned int VulkanRenderContext::rateDeviceCompatiblities(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, SwapChainSupportDetails& details) {
	VkPhysicalDeviceProperties deviceProperties;
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

void VulkanRenderContext::reAllocSwapchain() {
	vkDeviceWaitIdle(logicalDevice);

	deAllocSwapchain();

	VkSurfaceKHR surface = static_cast<VkSurfaceKHR>(window->getPlatformSurface());
	recreateSwapchain(surface);

	targetInFlight = 0;
}

void VulkanRenderContext::resizePickBuffer() {
	_pickBuffer->reserve(_width * _height * sizeof(PickUnit));
	_pickBuffer->commit();
}

void VulkanRenderContext::setSurface(void* params) {
	ContextParams* p{ reinterpret_cast<ContextParams*>(params) };
	VkSurfaceKHR surface = p->surface;
	vkInstance = p->vkInstance;
	initializeGraphicsDevice(surface, vkInstance);
}

void VulkanRenderContext::submitTransientBuffer(VkCommandBuffer commandBuffer) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	if( vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS )
		throw std::runtime_error("failed to submit command buffer!");

	// TODO: A fence would allow to schedule multiple transfers simultaneously and wait for all of them complete, instead of executing one at a time.That may give the driver more opportunities to optimize.
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
}

void VulkanRenderContext::submit( VkCommandBuffer buffer, VkFence vkFence, VkSemaphore renderFinished ) {
	newFrame = false;

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore waitSemaphores[1];
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.waitSemaphoreCount = 1;

	if (renderFinishedSemaphores.empty()) { // Wait for swapchain image acquire
		waitSemaphores[0] = imageAvailableSemaphore[targetInFlight];
	}
	else { // Wait for last submission to complete
		// TODO: Could pass additional params to allow parallel execution when appropriate. Serial is sometimes desired though (e.g. UI overlay)
		waitSemaphores[0] = renderFinishedSemaphores.back();
	}
	submitInfo.pWaitSemaphores = waitSemaphores;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buffer;

	VkSemaphore signalSemaphores[] = { renderFinished };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	// endFrame will wait the last of these semaphores to ensure all render ops are finished prior to present.
	renderFinishedSemaphores.push_back(renderFinished);

	vkResetFences(logicalDevice, 1, &vkFence);

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, vkFence) != VK_SUCCESS)
		throw std::runtime_error("failed to submit draw command buffer!");
}

void VulkanRenderContext::toggleFullScreen() {
	toggleFullscreen = true;
};

void VulkanRenderContext::waitOnIdle() {
	if(logicalDevice)
		vkDeviceWaitIdle(logicalDevice);
}

__declspec(dllexport)
	std::shared_ptr<IRenderContext> CreateRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs, bool headset) {
	std::shared_ptr<IRenderContext> context{ nullptr };
	if (headset)
		context = std::make_shared<OpenXRContext>(x, y, w, h, fs, headset);
	else 
		context = std::make_shared<VulkanRenderContext>(x, y, w, h, fs, headset);

	return context;
}

__declspec(dllexport)
	std::shared_ptr<IRenderContext> CloneRenderContext(const IRenderContext& obj) {
	std::shared_ptr<IRenderContext> context{ nullptr };
	if( dynamic_cast<const OpenXRContext*>(&obj) )
		context = std::make_shared<OpenXRContext>(obj);
	else
		context = std::make_shared<VulkanRenderContext>(obj);
		
	return context;
}

