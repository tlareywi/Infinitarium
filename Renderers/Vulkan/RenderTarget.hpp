//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "../../Engine/Texture.hpp"
#include "../../Engine/RenderPass.hpp"

#include "vulkan/vulkan.h"

#include <map>

class VulkanRenderContext;
class VulkanImage;
class VulkanRenderPass;

static inline void CheckVkResult( VkResult result ) {
	assert(result == VK_SUCCESS);
}

/// <summary>
/// Wrapper to enable smart pointer tracking of a Vulkan image view resource.
/// </summary>
struct ImageViewResource {
	ImageViewResource(VkDevice d, const VkImageViewCreateInfo& createInfo) : device(d) {
		CheckVkResult( vkCreateImageView(device, &createInfo, nullptr, &imageView) );
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
	VkDevice device{ VK_NULL_HANDLE };
	VkImageView imageView{ VK_NULL_HANDLE };
	ImageViewResource() : imageView{ VK_NULL_HANDLE }, device{ VK_NULL_HANDLE } {};
};

/// <summary>
/// Wrapper to enable smart pointer tracking of a Vulkan command buffer resource plus a fence to associate with it.
/// </summary>
struct CommandBufferResource {
	CommandBufferResource(VkDevice d, VkCommandBufferAllocateInfo& ci) : device(d), createInfo(ci) {
		CheckVkResult(vkAllocateCommandBuffers(d, &createInfo, &cmdBuffer));

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		CheckVkResult(vkCreateFence(d, &fenceInfo, nullptr, &fence));
	}
	VkCommandBuffer operator()() {
		return cmdBuffer;
	}
	VkFence getFence() {
		return fence;
	}
	~CommandBufferResource() {
		if (cmdBuffer != VK_NULL_HANDLE)
			vkFreeCommandBuffers(device, createInfo.commandPool, 1, &cmdBuffer);
		if( fence != VK_NULL_HANDLE)
			vkDestroyFence(device, fence, nullptr);
	}

private:
	VkDevice device{ VK_NULL_HANDLE };
	VkFence fence{ VK_NULL_HANDLE };
	VkCommandBuffer cmdBuffer{ VK_NULL_HANDLE };
	VkCommandBufferAllocateInfo createInfo;
	CommandBufferResource() : cmdBuffer{ VK_NULL_HANDLE }, device{ VK_NULL_HANDLE }, createInfo{} {};
};

/// <summary>
/// Wrapper to enable smart pointer tracking of a Vulkan framebuffer resource.
/// </summary>
struct FramebufferResource {
	FramebufferResource(VkDevice d, VkFramebufferCreateInfo& createInfoRef) : device(d), createInfo(createInfoRef) {
		CheckVkResult(vkCreateFramebuffer(device, &createInfo, nullptr, &framebuffer));
	}
	VkFramebuffer operator()() {
		return framebuffer;
	}
	VkFramebufferCreateInfo& getInfo() {
		return createInfo;
	}
	~FramebufferResource() {
		if (framebuffer != VK_NULL_HANDLE)
			vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	//void attach(VulkanRenderPass&, VulkanRenderTarget&);

private:
	VkDevice device;
	VkFramebufferCreateInfo createInfo;
	VkFramebuffer framebuffer{ VK_NULL_HANDLE };

	FramebufferResource() : framebuffer{ VK_NULL_HANDLE }, device{ VK_NULL_HANDLE }, createInfo{ VkFramebufferCreateInfo{} } {};
};

/// <summary>
/// Encapsulates the image resource(s) for a render target + framebuffer and command buffer. Associates framebuffer resource with a RenderPass.
/// </summary>
class VulkanRenderTarget : public IRenderTarget {
public:
	VulkanRenderTarget(const glm::uvec2&, Format, Type, Resource);
	VulkanRenderTarget(VulkanRenderContext&, const VkImageViewCreateInfo& createInfo);
	VulkanRenderTarget(VulkanRenderContext&, const VulkanRenderTarget& sourceTarget);
	VulkanRenderTarget(const IRenderTarget& obj) : IRenderTarget(obj) {}
	virtual ~VulkanRenderTarget();

	void initOffScreen(VulkanRenderContext& vkContext, const VulkanRenderTarget& sourceTarget );

	void prepare(IRenderContext&) override {};

	void descriptor(VkDescriptorImageInfo&, VulkanRenderContext&);
	VkFormat getPixelFormat() const;
	std::shared_ptr<FramebufferResource> getFramebuffer(const VulkanRenderPass& );
	VkCommandBuffer getCmdBuffer();
	VkFence getFence();

	VkSemaphore getSemaphore();
	const std::vector<VkImageView>& getFramebufferAttachments() const;

	void getData(const glm::uvec4&, void*) override {};

	void attach( VulkanRenderContext&, VulkanRenderPass& );

private:
	VkDevice device{ VK_NULL_HANDLE };
	std::shared_ptr<VulkanImage> backingImage{ nullptr };
	std::shared_ptr<ImageViewResource> imageView{ nullptr };
	std::shared_ptr<CommandBufferResource> cmdBuffer{ nullptr };

	VkSampler sampler{ nullptr };

	std::vector<VkImageView> framebufferAttachments;

	std::unordered_map<unsigned long long, std::shared_ptr<FramebufferResource>> renderPassAttachments;
 
	VkSemaphore renderFinished;
};

/// <summary>
/// Encapsultes a set of render targets with identical image properties. Example use would be for the swapchain targets. If we have a swapchain
/// of size 3 then we can create a RenderTargetStack of size 3 to represent our set of swapchain render targets. If no size is provided to
/// the consturctor then the static size is used, which is initialized to the swapchain size at context creation.
/// </summary>
class RenderTargetStack {
public:
	RenderTargetStack(VulkanRenderContext&, const std::vector<VkImageViewCreateInfo>&);
	RenderTargetStack::RenderTargetStack(VulkanRenderContext&, const VulkanRenderTarget&, uint8_t);
	~RenderTargetStack();

	VulkanRenderTarget& operator[](uint8_t indx);
	void attach(IRenderContext&, IRenderPass&, unsigned int = 0, unsigned int = 0);
	uint8_t size() { return static_cast<uint8_t>( renderTargets.size() ); }

private:
	std::vector<std::shared_ptr<VulkanRenderTarget>> renderTargets;
};
