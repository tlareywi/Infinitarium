#pragma once

#include "../../Engine/RenderPass.hpp"

#include "vulkan/vulkan.h"

///
/// \brief Metal implementation of RendePass
///
class VulkanRenderPass : public IRenderPass {
public:
	VulkanRenderPass() : renderPass(VK_NULL_HANDLE), device(VK_NULL_HANDLE) {}
	VulkanRenderPass(const IRenderPass& obj) : renderPass(VK_NULL_HANDLE), device(VK_NULL_HANDLE) , IRenderPass(obj) {}
	virtual ~VulkanRenderPass();

	void prepare(IRenderContext&) override;
	void begin(std::shared_ptr<IRenderContext>&) override;
	void end() override;

	VkRenderPass getVulkanRenderPass() {
		return renderPass;
	}

	VkCommandBuffer commandBuffer() {
		assert( swapChainIndx > 0 );
		return commandBuffers[swapChainIndx];
	}

private:
	VkRenderPass renderPass;
	VkDevice device;

	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers; 

	uint32_t swapChainIndx;

	VulkanRenderContext* activeContext;
};





