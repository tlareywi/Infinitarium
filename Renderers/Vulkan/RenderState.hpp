#pragma once

#include "../../Engine/RenderState.hpp"

#include "RenderContext.hpp"


class VulkanRenderState : public IRenderState {
public:
	VulkanRenderState();
	virtual ~VulkanRenderState();

	virtual void prepareImpl(IRenderContext& context, IRenderCommand& commmand);
	virtual void applyImpl(IRenderPass&);

	VkGraphicsPipelineCreateInfo& getPipelineState();
	VkPipelineLayoutCreateInfo& getPipelineLayoutState();
	VkPipeline getPipeline();
	VkDevice getDevice() {
		return device;
	}

private:
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkDevice device;

	VkViewport viewport;
	VkRect2D scissor;
	VkPipelineViewportStateCreateInfo viewportState{};
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	VkPipelineMultisampleStateCreateInfo multisampling{};
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	VkPipelineColorBlendStateCreateInfo colorBlending{};
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

	bool newPipeline;
};

