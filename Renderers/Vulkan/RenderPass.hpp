#pragma once

#include "../../Engine/RenderPass.hpp"
#include "RenderState.hpp"
#include "RenderContext.hpp"

#include "vulkan/vulkan.h"

///
/// \brief Metal implementation of RendePass
///
class VulkanRenderPass : public IRenderPass {
public:
	VulkanRenderPass() : renderPass(VK_NULL_HANDLE), device(VK_NULL_HANDLE), activeContext(nullptr), needDescriptorUpdate(false) {}
	VulkanRenderPass(const IRenderPass& obj) : renderPass(VK_NULL_HANDLE), device(VK_NULL_HANDLE), activeContext(nullptr), needDescriptorUpdate(false), IRenderPass(obj) {}
	virtual ~VulkanRenderPass();

	void prepare(IRenderContext&) override;
	void begin(std::shared_ptr<IRenderContext>&) override;
	void end() override;

	VkRenderPass getVulkanRenderPass() {
		return renderPass;
	}

	VkCommandBuffer commandBuffer() {
		// TEMP: won't always be tied to a swapchain buffer
		return commandBuffers[swapChainIndx];
	}

	const VkDescriptorSet* descriptor() {
		// TEMP: won't always be tied to a swapchain buffer
		return &descriptorSets[swapChainIndx];
	}

	std::vector<VkDescriptorSet>& descriptorSet() {
		return descriptorSets;
	}

	bool updateDescriptorSets() {
		return needDescriptorUpdate;
	}

	void refreshDescriptors( VulkanRenderState& );

private:
	VkRenderPass renderPass;
	VkDevice device;

	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers; 
	std::vector<VkDescriptorSet> descriptorSets;
	bool needDescriptorUpdate;

	uint32_t swapChainIndx;

	VulkanRenderContext* activeContext;

	VkDescriptorSetAllocateInfo descriptorAllocInfo{};
};





