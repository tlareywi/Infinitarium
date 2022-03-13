//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "DataBuffer.hpp"
#include "RenderContext.hpp"
#include "vulkan/vulkan.h"

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