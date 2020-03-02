#pragma once

#include "../../Engine/RenderState.hpp"

#include "RenderContext.hpp"


class VulkanRenderState : public IRenderState {
public:
	VulkanRenderState();
	virtual ~VulkanRenderState();

	virtual void prepareImpl(IRenderContext& context);
	virtual void applyImpl(IRenderPass&);

	VkGraphicsPipelineCreateInfo& getPipelineState();
	VkPipeline getPipeline();

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
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};

	bool newPipeline;
};

