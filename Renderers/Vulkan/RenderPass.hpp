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

	void prepare(std::shared_ptr<IRenderContext>&) override;
	void begin(std::shared_ptr<IRenderContext>&) override;
	void end() override;

	VkRenderPass getVulkanRenderPass() {
		return renderPass;
	}

private:
	VkRenderPass renderPass;
	VkDevice device;

	std::vector<VkFramebuffer> swapChainFramebuffers;
};





