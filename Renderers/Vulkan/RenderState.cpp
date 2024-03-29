//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "RenderState.hpp"
#include "RenderProgram.hpp"
#include "RenderPass.hpp"
#include "RenderCommand.hpp"

VulkanRenderState::VulkanRenderState() : newPipeline(true), graphicsPipeline(nullptr), pipelineLayout(nullptr) {
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr;
}

VulkanRenderState::~VulkanRenderState() {
	if (device) {
		if(graphicsPipeline)
			vkDestroyPipeline(device, graphicsPipeline, nullptr);
		if(pipelineLayout)
			vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	}
}

void VulkanRenderState::prepareImpl(IRenderContext& context, IRenderCommand& commmand) {
	viewport.x = (float)context.x();
	viewport.y = (float)context.y();
	viewport.width = (float)context.width();
	viewport.height = (float)context.height();
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset = { 0, 0 };
	scissor.extent = VkExtent2D{ context.width(), context.height() };

	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = polygonMode;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = cullMode;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = ((int)(sizeof(dynamicStates) / sizeof(*(dynamicStates))));
	dynamicState.pDynamicStates = dynamicStates;

	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pNext = nullptr;
	pipelineLayoutInfo.flags = 0;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	VulkanRenderContext& vkContext = dynamic_cast<VulkanRenderContext&>( context );
	device = vkContext.getVulkanDevice();

	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState; // Optional

	// Future use; create new pipeline derived on current (faster)
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	VulkanRenderCommand& vkRenderCommand = dynamic_cast<VulkanRenderCommand&>( commmand );
	pipelineInfo.pVertexInputState = vkRenderCommand.getVertexState();
	pipelineInfo.pInputAssemblyState = vkRenderCommand.getPrimitiveState();

	vkRenderCommand.updateDescriptors(context, *this);

	newPipeline = true;
}

void VulkanRenderState::applyImpl(IRenderPass& renderPass) {
	if (!newPipeline) return; // Nothing changed? No need to re-create pipeline

	if(graphicsPipeline)
		vkDestroyPipeline(device, graphicsPipeline, nullptr);
	if(pipelineLayout)
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout!");
	}

	pipelineInfo.layout = pipelineLayout;

	VulkanRenderPass* vkRenderPass = dynamic_cast<VulkanRenderPass*>(&renderPass);
	pipelineInfo.renderPass = vkRenderPass->getVulkanRenderPass();
	pipelineInfo.subpass = 0;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create graphics pipeline!");
	}

	newPipeline = false;
}

void VulkanRenderState::setCullMode(IRenderState::CullMode mode) {
	switch (mode) {
	case IRenderState::CullMode::Front:
		cullMode = VK_CULL_MODE_FRONT_BIT;
		break;
	case IRenderState::CullMode::Back:
		cullMode = VK_CULL_MODE_BACK_BIT;
		break;
	case IRenderState::CullMode::None:
	default:
		cullMode = VK_CULL_MODE_NONE;
		break;
	}
}

void VulkanRenderState::setPolygonMode(IRenderState::PolygonMode mode) {
	switch (mode) {
	case IRenderState::PolygonMode::Fill:
		polygonMode = VK_POLYGON_MODE_FILL;
		break;
	case IRenderState::PolygonMode::Line:
		polygonMode = VK_POLYGON_MODE_LINE;
		break;
	case IRenderState::PolygonMode::Point:
	default:
		polygonMode = VK_POLYGON_MODE_POINT;
		break;
	}
}

VkGraphicsPipelineCreateInfo& VulkanRenderState::getPipelineState() {
	return pipelineInfo;
}

VkPipelineLayoutCreateInfo& VulkanRenderState::getPipelineLayoutState() {
	return pipelineLayoutInfo;
}

VkPipeline VulkanRenderState::getPipeline() {
	return graphicsPipeline;
}

RENDERER_EXPORT
	std::shared_ptr<IRenderState> CreateRenderState() {
		return std::make_shared<VulkanRenderState>();
	}

