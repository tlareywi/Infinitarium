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
	VulkanRenderPass() : renderPass(VK_NULL_HANDLE), device(VK_NULL_HANDLE) {}
	VulkanRenderPass(const IRenderPass& obj) : renderPass(VK_NULL_HANDLE), device(VK_NULL_HANDLE), IRenderPass(obj) {}
	virtual ~VulkanRenderPass();

	void prepare(IRenderContext&) override;
	void begin(IRenderContext&) override;
	void end(IRenderContext&) override;

	VkRenderPass getVulkanRenderPass() {
		return renderPass;
	}

	VkCommandBuffer commandBuffer();

private:
	VkRenderPass renderPass;
	VkDevice device;

	VulkanRenderTarget* currentTarget{ nullptr };
};





