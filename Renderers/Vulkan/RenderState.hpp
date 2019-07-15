#pragma once

#include "../../Engine/RenderState.hpp"

#include "RenderContext.hpp"


class VulkanRenderState : public IRenderState {
public:
	VulkanRenderState();
	virtual ~VulkanRenderState();

	void commit(IRenderContext& context) override;

	VkGraphicsPipelineCreateInfo& getVulkanPipelineInfoRef() {
		return pipelineInfo;
	}

private:
	VkGraphicsPipelineCreateInfo pipelineInfo;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkDevice device;
};

