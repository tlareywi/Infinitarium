#include "RenderCommand.hpp"
#include "RenderContext.hpp"
#include "RenderPass.hpp"
#include "DataBuffer.hpp"

#include <glm/gtc/matrix_transform.hpp>

template<typename T, typename U> const U static inline convert(const T& t) {
	assert( sizeof(T) >= sizeof(U) );
	U retType;
	memcpy(&retType, &t, sizeof(U));
	return retType;
}

void VulkanRenderCommand::add(std::shared_ptr<IDataBuffer>& buffer) {
	IRenderCommand::add(buffer);

	if (buffer->getUsage() == IDataBuffer::Usage::Uniform) {

	}
	else if(buffer->getUsage() == IDataBuffer::Usage::Storage) {

	}
	else { //  IDataBuffer::Usage::VertexAttribute
		// First call, build the struct based on the buffers and primitives we have configured. 
		VkVertexInputBindingDescription bindingDescription{};
		VkVertexInputAttributeDescription attribDescription{};
		bindingDescription.binding = static_cast<uint32_t>(bindingDescriptions.size());
		bindingDescription.stride = buffer->getStride();
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		bindingDescriptions.emplace_back(std::move(bindingDescription));

		// TODO: Extend datapack to support multiple attributes.
		attribDescription.binding = static_cast<uint32_t>(attribDescriptions.size());
		attribDescription.location = static_cast<uint32_t>(attribDescriptions.size());
		attribDescription.format = static_cast<VkFormat>(buffer->getFormat());
		attribDescription.offset = 0; //offsetof(Vertex, pos);
		attribDescriptions.emplace_back(std::move(attribDescription));
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
	case Triangle:
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;

	case TriangleStrip:
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		break;

	case Line:
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		break;

	case Point:
	default:
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		break;
	}

	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
}

void VulkanRenderCommand::encode(IRenderPass& renderPass, IRenderState& state) {
	VulkanRenderState* vkRenderState{ dynamic_cast<VulkanRenderState*>(&state) };
	VulkanRenderPass* vkRenderPass{ dynamic_cast<VulkanRenderPass*>(&renderPass) };

	if (!vkRenderPass) {
		std::cout << "WARNING, failed cast to vkRenderPass." << std::endl;
		return;
	}

	vkCmdBindPipeline( vkRenderPass->commandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vkRenderState->getPipeline() );

	vkCmdBindDescriptorSets( vkRenderPass->commandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vkRenderState->getPipelineState().layout, 0, 1, &descriptors, 0, nullptr);

	uint32_t numBuffers{ static_cast<uint32_t>(dataBuffers.size()) };
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceSize> offsets;
	vertexBuffers.reserve(numBuffers);
	offsets.reserve(numBuffers);

	for (auto& buffer : dataBuffers) {
		if (buffer->getUsage() == IDataBuffer::Usage::Uniform)
			continue;
		VulkanBuffer* vulkanBuffer{ dynamic_cast<VulkanBuffer*>(buffer.get()) };
		vertexBuffers.push_back(vulkanBuffer->getVkBuffer());
		offsets.push_back( 0 );
	}

	if( !vertexBuffers.empty() )
		vkCmdBindVertexBuffers(vkRenderPass->commandBuffer(), 1, (uint32_t)vertexBuffers.size(), vertexBuffers.data(), offsets.data());

	if( instanceCount > 0 )
		vkCmdDraw( vkRenderPass->commandBuffer(), vertexCount, instanceCount, 0, 0 );
}

void VulkanRenderCommand::updateDescriptors(IRenderContext& context, IRenderState& state) {
	VulkanRenderContext& vkContext{ dynamic_cast<VulkanRenderContext&>(context) };
	VulkanRenderState& vkState{ dynamic_cast<VulkanRenderState&>(state) };
	device = vkState.getDevice();

	// TODO: Refactor to AllocateDescriptors /////////////////////////////////////////////////////////////////////////
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
	for (auto& buffer : dataBuffers) {
		VkDescriptorSetLayoutBinding layoutBinding;
		layoutBinding.binding = (uint32_t)layoutBindings.size() + 1; // Binding 0 reserved for injected uniforms
		layoutBinding.descriptorCount = 1;
		layoutBinding.pImmutableSamplers = nullptr;
		layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		switch (buffer->getUsage()) {
		case  IDataBuffer::Usage::Uniform:
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layoutBinding.binding = 0;
			break;
		case IDataBuffer::Usage::Storage:
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;
		default:
			continue;
		}

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
	descriptorAllocInfo.descriptorSetCount = 1; // static_cast<uint32_t>(swapChainFramebuffers.size());
	descriptorAllocInfo.pSetLayouts = &descriptorSetLayout;

	if (vkAllocateDescriptorSets(vkState.getDevice(), &descriptorAllocInfo, &descriptors) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
	// END Refactor to AllocateDescriptors /////////////////////////////////////////////////////////////////////////

	std::vector<VkWriteDescriptorSet> writeDescriptorSets;
	std::vector<VkDescriptorBufferInfo> bufferDescriptors;
	bufferDescriptors.reserve(dataBuffers.size());
	writeDescriptorSets.reserve(dataBuffers.size());
	unsigned int binding{ 1 };  // Binding 0 reserved for injected uniforms

	for (auto& buffer : dataBuffers) {
		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptors;
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

		descriptorWrite.pBufferInfo = &(bufferDescriptors[binding - 1]);
		descriptorWrite.dstBinding = binding++;

		switch (buffer->getUsage()) {
		case  IDataBuffer::Usage::Uniform:
			descriptorWrite.dstBinding = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
		case IDataBuffer::Usage::Storage:
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;
		default:
			continue;
		}

		writeDescriptorSets.emplace_back(std::move(descriptorWrite));
	}

	vkUpdateDescriptorSets(vkState.getDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, NULL);
}

__declspec(dllexport)
	std::shared_ptr<IRenderCommand> CreateRenderCommand() {
		return std::make_shared<VulkanRenderCommand>();
	}