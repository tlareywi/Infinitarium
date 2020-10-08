#pragma once

#include "../../Engine/RenderCommand.hpp"

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
	virtual ~VulkanRenderCommand() {
		if(descriptorSetLayout)
			vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	}

	void add(std::shared_ptr<IDataBuffer>&) override;

	void encode(IRenderPass& renderPass, IRenderState& state) override;
	void setPrimitiveType(PrimitiveType) override;

	const VkPipelineInputAssemblyStateCreateInfo* getPrimitiveState() const;
	const VkPipelineVertexInputStateCreateInfo* getVertexState();
	void updateDescriptors( IRenderContext&, IRenderState& );

private:
	VkDevice device{nullptr};
	VkDescriptorSet descriptors{nullptr};
	VkDescriptorSetLayout descriptorSetLayout{nullptr};
	VkDescriptorSetAllocateInfo descriptorAllocInfo{};

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> attribDescriptions;
};

