//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "../../Engine/RenderCommand.hpp"
#include "RenderContext.hpp"
#include "RenderState.hpp"

#include "vulkan/vulkan.h"

///
/// brief Metal implementation of RenderCommand
///
class VulkanRenderCommand : public IRenderCommand {
public:
	VulkanRenderCommand() {
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
	}
	virtual ~VulkanRenderCommand();

	void add(std::shared_ptr<IDataBuffer>&) override;

	void encode(IRenderPass& renderPass, IRenderState& state) override;
	void setPrimitiveType(PrimitiveType) override;

	const VkPipelineInputAssemblyStateCreateInfo* getPrimitiveState() const;
	const VkPipelineVertexInputStateCreateInfo* getVertexState();
	void updateDescriptors( IRenderContext&, IRenderState& );

private:
	void allocateDescriptors(VulkanRenderContext& vkContext, VulkanRenderState& vkState);

	bool init{ false };

	VkDevice device{nullptr};
	VkDescriptorSet descriptorSet{nullptr};
	VkDescriptorSetLayout descriptorSetLayout{nullptr};
	VkDescriptorSetAllocateInfo descriptorAllocInfo{};

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> attribDescriptions;
};

