#pragma once

#include "../../Engine/RenderState.hpp"

#include "RenderContext.hpp"


class VulkanRenderState : public IRenderState {
public:
	VulkanRenderState();
	virtual ~VulkanRenderState();

	void prepareImpl(IRenderContext& context, IRenderCommand& commmand) override;
	void applyImpl(IRenderPass&) override;

	void setCullMode(IRenderState::CullMode) override;

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

	VkCullModeFlags cullMode{ VK_CULL_MODE_NONE };

	bool newPipeline;
};

