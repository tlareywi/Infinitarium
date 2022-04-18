//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

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
	void setPolygonMode(IRenderState::PolygonMode) override;

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
	VkPolygonMode polygonMode{ VK_POLYGON_MODE_FILL };

	// TODO: For convinience currently. Could revisit for perf tuning.
	VkDynamicState dynamicStates[1] = {
		VK_DYNAMIC_STATE_VIEWPORT
	};
	VkPipelineDynamicStateCreateInfo dynamicState = {};

	bool newPipeline;
};

