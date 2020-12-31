#pragma once

#include "DataBuffer.hpp"

class VulkanImage {
public:
	VulkanImage(const VulkanRenderContext&, const VkImageCreateInfo&);
	~VulkanImage();

	VkImage const getImage() {
		return vkImage;
	}
	void copy(VkCommandBuffer commandBuffer, const VulkanBuffer&, const glm::ivec4& rect );
	void transitionLayout(VkCommandBuffer, VkImageLayout, VkImageLayout);

private:
	VulkanImage() : vkImage{ nullptr }, imageMemory{ nullptr } {}
	
	VkDevice vkDevice{nullptr};
	VkImage vkImage;
	VkDeviceMemory imageMemory;
};