//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "../../Engine/ApplicationWindow.hpp"
#include "../../Engine/RenderContext.hpp"
#include "RenderTarget.hpp"

#include "vulkan/vulkan.h"

#include <vector>
#include <functional>
#include <atomic>

#if WIN32
#include <windows.h>
#endif

#include <GLFW/glfw3.h>

#if USE_OPENXR
#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr.h>
#include <openxr_platform.h>
#else
	using XrInstance = VkInstance;
	using XrSystemId = uint32_t;
#endif

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities{};
	std::vector<VkSurfaceFormatKHR> formats{};
	std::vector<VkPresentModeKHR> presentModes{};
};

///
///
///
class VulkanRenderContext : public IRenderContext {
public:
	struct ContextParams {
		VkSurfaceKHR surface;
		VkInstance vkInstance;
		XrInstance xrInstance;
		XrSystemId xrSystemId;
	};

	VulkanRenderContext(const IRenderContext& obj);
	VulkanRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs, bool headset);
	virtual ~VulkanRenderContext();

	void* getSurface() override;
	void setSurface(void*) override;
	void pauseRendering(bool) override;
	void beginFrame() override;
	void endFrame() override;
	void waitOnIdle() override;
	void toggleFullScreen() override;
	void resizePickBuffer() override;

	virtual VkImageLayout swapchainLayout() {
		return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	}
	virtual VkImageLayout attachmentLayout() {
		return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	virtual VulkanRenderTarget& getSwapchainTarget();
	virtual void attachSwapchain(IRenderPass& renderPass);
	virtual void submit(VkCommandBuffer, VkFence, std::vector<VkSemaphore>&, std::vector<VkSemaphore>&);
	virtual GLFWwindow* getWindow() {
		return static_cast<GLFWwindow*>(window->getPlatformWindow());
	}
	virtual size_t numImages() {
		return swapchainTargets->size();
	}

	VkCommandBuffer allocTransientBuffer();
	void submitTransientBuffer(VkCommandBuffer);

	VkDevice const getVulkanDevice() const {
		return logicalDevice;
	}
	VkPhysicalDevice const getPhysicalDevice() const {
		return physicalDevice;
	}
	VkCommandPool const getCommandPool() const {
		return commandPool;
	}
	VkDescriptorPool const getDescriptorPool() {
		return descriptorPool;
	}
	bool invalidDescriptorPool() {
		return resetDescriptors;
	}
	void getVulkanSwapchainInfo(VkSwapchainCreateInfoKHR& info) {
		memcpy(&info, &swapchainCreateInfo, sizeof(info));
	}
	VkQueue const getGraphicsQueue() {
		return graphicsQueue;
	}
	VkInstance const getVkInstance() {
		return vkInstance;
	}
	bool isNewFrame() {
		return newFrame;
	}
	uint32_t getTargetInFlight() const {
		return targetInFlight;
	}

protected:
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkSwapchainCreateInfoKHR swapchainCreateInfo{};
	VkDeviceCreateInfo& createDeviceQueueInfo(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
	void createDeviceQueue();
	void createDescriptorPool();
	unsigned int graphicsQueueIndx{ 0 };
	VkQueue graphicsQueue;
	VkInstance vkInstance;

private:
	void initializeGraphicsDevice(const VkSurfaceKHR&, const VkInstance&);
	unsigned int rateDeviceCompatiblities(const VkPhysicalDevice&, const VkSurfaceKHR&, SwapChainSupportDetails&);
	void createDeviceGraphicsQueue(const VkPhysicalDevice&, const VkSurfaceKHR&);
	bool checkDeviceExtensionSupport(const VkPhysicalDevice&);
	bool querySwapChain(const VkPhysicalDevice&, const VkSurfaceKHR&, SwapChainSupportDetails& details);
	void createSwapChain(const VkSurfaceKHR& surface);
	void reAllocSwapchain();
	
	// Device resources
	VkCommandPool commandPool = nullptr;
	VkDescriptorPool descriptorPool = nullptr;
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	VkPhysicalDeviceFeatures deviceFeatures = {};
	VkDeviceCreateInfo deviceCreateInfo = {};

	// Swapchain
	SwapChainSupportDetails swapChainCaps{};
	VkSwapchainKHR swapChain;
	std::unique_ptr<RenderTargetStack> swapchainTargets;

	const std::vector<const char*> deviceExtensions = { 
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
#ifndef __APPLE__
		,VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME // For shader debugging
#endif
	};

	std::vector<VkSemaphore> imageAvailableSemaphore;
	std::vector<VkSemaphore> renderFinishedSemaphores;

	uint32_t targetInFlight{ 0 };

	std::atomic<bool> renderingPaused;
	std::atomic<bool> toggleFullscreen;
	std::function<void(VkSurfaceKHR&)> recreateSwapchain;
	void deAllocSwapchain();

	bool newFrame{ false };
	bool resetDescriptors{ true };
};

