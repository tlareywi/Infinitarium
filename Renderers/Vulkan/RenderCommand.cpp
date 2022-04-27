//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "RenderCommand.hpp"
#include "RenderContext.hpp"
#include "RenderPass.hpp"
#include "DataBuffer.hpp"
#include "Texture.hpp"

#include <glm/gtc/matrix_transform.hpp>

template<typename T, typename U> const U static inline convert(const T& t) {
	assert( sizeof(T) >= sizeof(U) );
	U retType;
	memcpy(&retType, &t, sizeof(U));
	return retType;
}

VulkanRenderCommand::~VulkanRenderCommand() {
	if (descriptorSetLayout)
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

	dataBuffers.clear();
}

void VulkanRenderCommand::add(std::shared_ptr<IDataBuffer>& buffer) {
	IRenderCommand::add(buffer);

	if (buffer->getUsage() == IDataBuffer::Usage::VertexAttribute) {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = static_cast<uint32_t>(bindingDescriptions.size());
		bindingDescription.stride = buffer->getStride();
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		bindingDescriptions.emplace_back(std::move(bindingDescription));

		for (auto& attrib : attributes) {
			VkVertexInputAttributeDescription attribDescription{};
			attribDescription.binding = bindingDescription.binding;
			attribDescription.location = attrib.location;
			attribDescription.offset = attrib.offset;

			switch (attrib.type) {
			case AttributeType::Position:
				attribDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
			case AttributeType::Color:
				attribDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
				break;
			case AttributeType::Normal:
				attribDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
				break;
			case AttributeType::UV:
				attribDescription.format = VK_FORMAT_R32G32_SFLOAT;
				break;
			}

			attribDescriptions.emplace_back(std::move(attribDescription));
		}
	}
}

const VkPipelineInputAssemblyStateCreateInfo* VulkanRenderCommand::getPrimitiveState() const {
	return &inputAssembly;
}

const VkPipelineVertexInputStateCreateInfo* VulkanRenderCommand::getVertexState() {
	if( vertexInputInfo.sType == VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO )
		return &vertexInputInfo;

	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attribDescriptions.data();

	return &vertexInputInfo;
}

void VulkanRenderCommand::setPrimitiveType(PrimitiveType t) {
	switch (t) {
	case PrimitiveType::Triangle:
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;

	case PrimitiveType::TriangleStrip:
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		break;

	case PrimitiveType::Line:
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		break;

	case PrimitiveType::Point:
	default:
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		break;
	}

	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
}

void VulkanRenderCommand::encode(IRenderPass& renderPass, IRenderState& state) {
	VulkanRenderState& vkRenderState{ dynamic_cast<VulkanRenderState&>(state) };
	VulkanRenderPass& vkRenderPass{ dynamic_cast<VulkanRenderPass&>(renderPass) };

	vkCmdBindPipeline( vkRenderPass.commandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vkRenderState.getPipeline() );

	vkCmdBindDescriptorSets( vkRenderPass.commandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vkRenderState.getPipelineState().layout, 0, 1, &descriptorSet, 0, nullptr);

	uint32_t numBuffers{ static_cast<uint32_t>(dataBuffers.size()) };
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceSize> offsets;
	vertexBuffers.reserve(numBuffers);
	offsets.reserve(numBuffers);

	for (auto& buffer : dataBuffers) {
		if (buffer->getUsage() != IDataBuffer::Usage::VertexAttribute)
			continue;
		VulkanBuffer* vulkanBuffer{ dynamic_cast<VulkanBuffer*>(buffer.get()) };
		vertexBuffers.push_back(vulkanBuffer->getVkBuffer());
		offsets.push_back( 0 );
	}

	if( !vertexBuffers.empty() )
		vkCmdBindVertexBuffers(vkRenderPass.commandBuffer(), 0, (uint32_t)vertexBuffers.size(), vertexBuffers.data(), offsets.data());

	if( instanceCount > 0 )
		vkCmdDraw( vkRenderPass.commandBuffer(), vertexCount, instanceCount, 0, 0 );
}

void VulkanRenderCommand::allocateDescriptors(VulkanRenderContext& vkContext, VulkanRenderState& vkState) {
	if (descriptorSetLayout)
		vkDestroyDescriptorSetLayout(vkState.getDevice(), descriptorSetLayout, nullptr);

	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
	for (auto& buffer : dataBuffers) {
        // TODO: These should probably be allocated on a global basis and assiciated with the resource; not per RenderCommand for a non-unique
        // resource. This is just chewing through the descriptor pool.
		VkDescriptorSetLayoutBinding layoutBinding;
		layoutBinding.binding = (uint32_t)layoutBindings.size() + 3; // Binding 0 reserved for injected uniforms, 1 reserved for pick buffer, 2 for Light/Post Process
		layoutBinding.descriptorCount = 1;
		layoutBinding.pImmutableSamplers = nullptr;

		switch (buffer->getUsage()) {
		case  IDataBuffer::Usage::Uniform:
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			layoutBinding.binding = 0;
			break;
		case  IDataBuffer::Usage::Pick:
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			layoutBinding.binding = 1;
			break;
		case  IDataBuffer::Usage::PostProcess:
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			layoutBinding.binding = 2;
			break;
		case IDataBuffer::Usage::Storage:
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		default:
			continue;
		}

		layoutBindings.push_back(layoutBinding);
	}
 
	for (auto& texture : textures) {
		VkDescriptorSetLayoutBinding layoutBinding;
		layoutBinding.binding = (uint32_t)layoutBindings.size();
		layoutBinding.descriptorCount = 1;
		layoutBinding.pImmutableSamplers = nullptr;
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		layoutBindings.push_back(layoutBinding);
	}

	for (auto& sampler : samplers) {
		VkDescriptorSetLayoutBinding layoutBinding;
		layoutBinding.binding = (uint32_t)layoutBindings.size();
		layoutBinding.descriptorCount = 1;
		layoutBinding.pImmutableSamplers = nullptr;
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		layoutBindings.push_back(layoutBinding);
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = (uint32_t)layoutBindings.size();
	layoutInfo.pBindings = layoutBindings.data();

	if (vkCreateDescriptorSetLayout(vkState.getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	VkPipelineLayoutCreateInfo& layoutCreateInfo{ vkState.getPipelineLayoutState() };
	layoutCreateInfo.setLayoutCount = 1;
	layoutCreateInfo.pSetLayouts = &descriptorSetLayout;

	descriptorAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorAllocInfo.descriptorPool = vkContext.getDescriptorPool();
	descriptorAllocInfo.descriptorSetCount = 1;
	descriptorAllocInfo.pSetLayouts = &descriptorSetLayout;

	VkResult err{ vkAllocateDescriptorSets(vkState.getDevice(), &descriptorAllocInfo, &descriptorSet) };
	if (err != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
}

void VulkanRenderCommand::updateDescriptors(IRenderContext& context, IRenderState& state) {
	VulkanRenderContext& vkContext{ dynamic_cast<VulkanRenderContext&>(context) };
	VulkanRenderState& vkState{ dynamic_cast<VulkanRenderState&>(state) };

	if (!init || vkContext.invalidDescriptorPool() ) {
		allocateDescriptors(vkContext, vkState);
		init = true;
	}

	device = vkState.getDevice();

	std::vector<VkWriteDescriptorSet> writeDescriptorSets;
	std::vector<VkDescriptorBufferInfo> bufferDescriptors;
	bufferDescriptors.reserve(dataBuffers.size());
	writeDescriptorSets.reserve(dataBuffers.size() + textures.size() + samplers.size());
	unsigned int binding{ 3 };  // Binding 0 reserved for injected uniforms, 1 reserved for pick buffer

	for (auto& buffer : dataBuffers) {
		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSet;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional

		VkDescriptorBufferInfo bufferInfo{};
		VulkanBuffer* vulkanBuffer{ dynamic_cast<VulkanBuffer*>(buffer.get()) };
		bufferInfo.buffer = vulkanBuffer->getVkBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = vulkanBuffer->length();
		bufferDescriptors.emplace_back(std::move(bufferInfo));

		descriptorWrite.pBufferInfo = &(bufferDescriptors[binding - 3]);
		descriptorWrite.dstBinding = binding++;

		switch (buffer->getUsage()) {
		case  IDataBuffer::Usage::Uniform:
			descriptorWrite.dstBinding = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
		case  IDataBuffer::Usage::Pick:
			descriptorWrite.dstBinding = 1;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;
		case  IDataBuffer::Usage::PostProcess:
			descriptorWrite.dstBinding = 2;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;
		case IDataBuffer::Usage::Storage:
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;
		default:
			continue;
		}

		writeDescriptorSets.emplace_back(std::move(descriptorWrite));
	}

	std::vector<VkDescriptorImageInfo> textureDescriptors;
	textureDescriptors.reserve(textures.size());
	for (auto& texture : textures) {
		VulkanTexture* vkTexture{ dynamic_cast<VulkanTexture*>(texture.get()) };
		VkDescriptorImageInfo imageInfo{};
		vkTexture->descriptor(imageInfo);
		textureDescriptors.emplace_back(imageInfo);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSet;
		descriptorWrite.dstBinding = static_cast<uint32_t>(writeDescriptorSets.size());
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = &(textureDescriptors.back());

		writeDescriptorSets.emplace_back(std::move(descriptorWrite));
	}

	std::vector<VkDescriptorImageInfo> samplerDescriptors;
	samplerDescriptors.reserve(samplers.size());
	for (auto& sampler : samplers) {
		VulkanRenderTarget* vkTarget{ dynamic_cast<VulkanRenderTarget*>(sampler.get()) };
		VkDescriptorImageInfo imageInfo{};
		vkTarget->descriptor(imageInfo, vkContext);
		samplerDescriptors.emplace_back(imageInfo);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSet;
		descriptorWrite.dstBinding = static_cast<uint32_t>(writeDescriptorSets.size());
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = &(samplerDescriptors.back());

		writeDescriptorSets.emplace_back(std::move(descriptorWrite));
	}

	vkUpdateDescriptorSets(vkState.getDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, NULL);
}

RENDERER_EXPORT std::shared_ptr<IRenderCommand> CreateRenderCommand() {
	return std::make_shared<VulkanRenderCommand>();
}
