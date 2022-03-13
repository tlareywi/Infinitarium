//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "Image.hpp"

VulkanImage::VulkanImage(const VulkanRenderContext& vkContext, const VkImageCreateInfo& imageInfo) {
	vkDevice = vkContext.getVulkanDevice();

	if (vkCreateImage(vkDevice, &imageInfo, nullptr, &vkImage) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements{};
	vkGetImageMemoryRequirements(vkDevice, vkImage, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = VulkanBuffer::findMemoryType(vkContext, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(vkDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(vkDevice, vkImage, imageMemory, 0);
}

VulkanImage::~VulkanImage() {
	if (vkDevice) {
		vkDestroyImage(vkDevice, vkImage, nullptr);
		vkFreeMemory(vkDevice, imageMemory, nullptr);
	}
}

void VulkanImage::copy(VkCommandBuffer commandBuffer, const VulkanBuffer& vkBuffer, const glm::ivec4& rect) {
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset.x = rect.x; region.imageOffset.y = rect.y; region.imageOffset.z = 0;
	region.imageExtent.width = rect.z; region.imageExtent.height = rect.w; region.imageExtent.depth = 1;

	vkCmdCopyBufferToImage( commandBuffer, vkBuffer.getStagingBuffer(), vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region );
}

void VulkanImage::transitionLayout(VkCommandBuffer commandBuffer, VkImageLayout source, VkImageLayout target) {
	VkPipelineStageFlags sourceMask;
	VkPipelineStageFlags destMask;

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = source;
	barrier.newLayout = target;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = vkImage;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	if (source == VK_IMAGE_LAYOUT_UNDEFINED && target == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (source == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && target == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
		throw std::invalid_argument("unsupported layout transition!");

	vkCmdPipelineBarrier( commandBuffer, sourceMask, destMask, 0, 0, nullptr, 0, nullptr, 1, &barrier );
}