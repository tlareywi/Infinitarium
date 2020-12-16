#pragma once

#include "../../Engine/ApplicationWindow.hpp"
#include "../../Engine/RenderContext.hpp"
#include "RenderTarget.hpp"

#include "vulkan/vulkan.h"

#include <vector>
#include <functional>

#include <windows.h>

#include <GLFW/glfw3.h>

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_VULKAN

#include <openxr.h>
#include <openxr_platform.h>

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

	virtual VkImageLayout swapchainLayout() {
		return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	}
	virtual VkImageLayout attachmentLayout() {
		return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	virtual VulkanRenderTarget& getSwapchainTarget();
	virtual void attachTargets(IRenderPass& renderPass);
	virtual void submit(VkCommandBuffer, VkFence, VkSemaphore);

	void submit(VkSubmitInfo&);

	VkDevice const getVulkanDevice() {
		return logicalDevice;
	}
	VkPhysicalDevice const getPhysicalDevice() {
		return physicalDevice;
	}
	VkCommandPool const getCommandPool() {
		return commandPool;
	}
	VkDescriptorPool const getDescriptorPool() {
		return descriptorPool;
	}
	void getVulkanSwapchainInfo(VkSwapchainCreateInfoKHR& info) {
		memcpy(&info, &swapchainCreateInfo, sizeof(info));
	}
	virtual size_t numImages() {
		return swapchainTargets.size();
	}
	VkQueue const getGraphicsQueue() {
		return graphicsQueue;
	}
	VkInstance const getVkInstance() {
		return vkInstance;
	}
	GLFWwindow* getWindow() {
		return static_cast<GLFWwindow*>(window->getPlatformWindow());
	}

protected:
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkSwapchainCreateInfoKHR swapchainCreateInfo{};
	VkDeviceCreateInfo& createDeviceQueueInfo(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
	void createDeviceQueue();
	void createDescriptorPool(uint32_t);
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
	VkCommandPool commandPool;
	VkDescriptorPool descriptorPool;
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	VkPhysicalDeviceFeatures deviceFeatures = {};
	VkDeviceCreateInfo deviceCreateInfo = {};

	// Swapchain
	SwapChainSupportDetails swapChainCaps{};
	VkSwapchainKHR swapChain;
	std::vector<VulkanRenderTarget> swapchainTargets;
	std::vector<VkCommandBuffer> commandQ;

	const std::vector<const char*> deviceExtensions = { 
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME
	};

	std::vector<VkSemaphore> imageAvailableSemaphore;
	std::vector<VkSemaphore> renderFinishedSemaphores;

	uint32_t targetInFlight{ 0 };

	std::atomic<bool> renderingPaused;
	std::atomic<bool> toggleFullscreen;
	std::function<void(VkSurfaceKHR&)> recreateSwapchain;
	void deAllocSwapchain();
};

