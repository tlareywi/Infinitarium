#pragma once

#include "../../Engine/RenderContext.hpp"

#include "vulkan/vulkan.h"

#include <vector>

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
	VulkanRenderContext(const IRenderContext& obj);
	VulkanRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs);
	virtual ~VulkanRenderContext();

	void* getSurface() override;
	void setSurface(void*, void*) override;

	VkDevice getVulkanDevice() {
		return logicalDevice;
	}

	VkPhysicalDevice getPhysicalDevice() {
		return physicalDevice;
	}

	VkCommandPool getCommandPool() {
		return commandPool;
	}

	VkDescriptorPool getDescriptorPool() {
		return descriptorPool;
	}

	void getVulkanSwapchainInfo( VkSwapchainCreateInfoKHR& info ) {
		memcpy(&info, &swapchainCreateInfo, sizeof(info));
	}

	uint32_t nextSwapChainTarget();

	const std::vector<VkImageView>& getImageViews() {
		return swapChainImageViews;
	}

	void submit( VkSubmitInfo& );
	void submit( VkCommandBuffer&, uint32_t );
	void present( uint32_t );

private:
	void initializeGraphicsDevice( const VkSurfaceKHR&, const VkInstance& );
	unsigned int rateDeviceCompatiblities(const VkPhysicalDevice&, const VkSurfaceKHR&, SwapChainSupportDetails&);
	void createDeviceGraphicsQueue(const VkPhysicalDevice&, const VkSurfaceKHR&);
	bool checkDeviceExtensionSupport(const VkPhysicalDevice&);
	bool querySwapChain(const VkPhysicalDevice&, const VkSurfaceKHR&, SwapChainSupportDetails& details);
	void createSwapChain( const VkSurfaceKHR& surface );
	void createDescriptorPool();
	
	// Device resources
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkQueue graphicsQueue;
	VkCommandPool commandPool;
	VkDescriptorPool descriptorPool;

	// Swapchain
	SwapChainSupportDetails swapChainCaps{};
	VkSwapchainCreateInfoKHR swapchainCreateInfo{};
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	unsigned int graphicsQIndx;

	const std::vector<const char*> deviceExtensions = { 
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME
	};

	std::vector<VkSemaphore> imageAvailableSemaphore;
	std::vector<VkSemaphore> renderFinishedSemaphore;
	std::vector<VkFence> imagesInFlight;
	std::vector<VkFence> inFlightFences;

	unsigned short currentSwapFrame;
};

