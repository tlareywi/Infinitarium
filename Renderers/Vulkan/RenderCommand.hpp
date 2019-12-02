#pragma once

#include "../../Engine/RenderCommand.hpp"

#include "vulkan/vulkan.h"

///
/// brief Metal implementation of RenderCommand
///
class VulkanRenderCommand : public IRenderCommand {
public:
	void encode(IRenderPass& renderPass, IRenderState& state) override;
	void setPrimitiveType(PrimitiveType) override;

private:
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
};

