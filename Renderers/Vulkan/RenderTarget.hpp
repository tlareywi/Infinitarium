#pragma once

#include "../../Engine/Texture.hpp"
#include "../../Engine/RenderPass.hpp"

#include "vulkan/vulkan.h"

/// <summary>
/// Wrapper to enable smart pointer tracking of a Vulkan image view resource.
/// </summary>
struct ImageViewResource {
	ImageViewResource(VkDevice d, VkImageViewCreateInfo& createInfo) : device(d) {
		assert(vkCreateImageView(device, &createInfo, nullptr, &imageView) == VK_SUCCESS);
	}
	VkImageView operator()() {
		return imageView;
	}
	~ImageViewResource() {
		if (imageView != VK_NULL_HANDLE) {
			vkDestroyImageView(device, imageView, nullptr);
		}
	}

private:
	VkDevice device;
	VkImageView imageView{ VK_NULL_HANDLE };
	ImageViewResource() : imageView{ VK_NULL_HANDLE }, device{ VK_NULL_HANDLE } {};
};

/// <summary>
/// Wrapper to enable smart pointer tracking of a Vulkan framebuffer resource.
/// </summary>
struct FramebufferResource {
	FramebufferResource(VkDevice d, VkFramebufferCreateInfo& createInfo) : device(d) {
		assert(vkCreateFramebuffer(device, &createInfo, nullptr, &framebuffer) == VK_SUCCESS);
	}
	VkFramebuffer operator()() {
		return framebuffer;
	}
	~FramebufferResource() {
		if (framebuffer != VK_NULL_HANDLE) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}
	}

private:
	VkDevice device;
	VkFramebuffer framebuffer{ VK_NULL_HANDLE };
	FramebufferResource() : framebuffer{ VK_NULL_HANDLE }, device{ VK_NULL_HANDLE } {};
};

/// <summary>
/// Wrapper to enable smart pointer tracking of a Vulkan command buffer resource.
/// </summary>
struct CommandBufferResource {
	CommandBufferResource(VkDevice d, VkCommandBufferAllocateInfo& ci) : device(d), createInfo(ci) {
		assert(vkAllocateCommandBuffers(d, &createInfo, &cmdBuffer) == VK_SUCCESS);
	}
	VkCommandBuffer operator()() {
		return cmdBuffer;
	}
	~CommandBufferResource() {
		if (cmdBuffer != VK_NULL_HANDLE) {
			vkFreeCommandBuffers(device, createInfo.commandPool, 1, &cmdBuffer);
		}
	}

private:
	VkDevice device;
	VkCommandBuffer cmdBuffer{ VK_NULL_HANDLE };
	VkCommandBufferAllocateInfo createInfo;
	CommandBufferResource() : cmdBuffer{ VK_NULL_HANDLE }, device{ VK_NULL_HANDLE }, createInfo{} {};
};

/// <summary>
/// Encapsulates the image resource(s) for a render target + framebuffer and command buffer. Associates framebuffer resource with a RenderPass.
/// </summary>
class VulkanRenderTarget : public IRenderTarget {
public:
	VulkanRenderTarget(const glm::uvec2&, Format, Type, Resource);
	VulkanRenderTarget(VkDevice logicalDevice, VkImageViewCreateInfo& createInfo);
	VulkanRenderTarget(const IRenderTarget& obj) : IRenderTarget(obj) {}
	virtual ~VulkanRenderTarget();

	void prepare(IRenderContext&) override {};
	void attach(IRenderContext&, IRenderPass&);

	VkFormat getPixelFormat();

	VkFramebuffer getFramebuffer();
	VkCommandBuffer getCmdBuffer();

	void getData(const glm::uvec4&, void*) override {};

private:
	std::shared_ptr<ImageViewResource> colorView;
	std::shared_ptr<ImageViewResource> depthView;
	std::shared_ptr<FramebufferResource> framebuffer;
	std::shared_ptr<CommandBufferResource> cmdBuffer;
};