#pragma once

#include "../../Engine/RenderContext.hpp"

#include "vulkan/vulkan.h"

#include <vector>

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

///
///
///
class VulkanRenderContext : public IRenderContext {
public:
	VulkanRenderContext(const IRenderContext& obj);
	VulkanRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs);
	virtual ~VulkanRenderContext();

	void* getSurface() override;
	void setSurface(void*) override;

	VkDevice getVulkanDevice() {
		return logicalDevice;
	}

private:
	void initializeGraphicsDevice( const VkSurfaceKHR& );
	unsigned int rateDeviceCompatiblities(const VkPhysicalDevice&, const VkSurfaceKHR&, SwapChainSupportDetails&);
	void createDeviceGraphicsQueue(const VkPhysicalDevice&, const VkSurfaceKHR&);
	bool checkDeviceExtensionSupport(const VkPhysicalDevice&);
	bool querySwapChain(const VkPhysicalDevice&, const VkSurfaceKHR&, SwapChainSupportDetails& details);
	void createSwapChain( const VkSurfaceKHR& surface );
	
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkQueue graphicsQueue;
	SwapChainSupportDetails swapChainCaps;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	unsigned int graphicsQIndx;

	const std::vector<const char*> deviceExtensions = { 
		VK_KHR_SWAPCHAIN_EXTENSION_NAME 
	};
};

