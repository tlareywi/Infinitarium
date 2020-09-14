#include "RenderCommand.hpp"
#include "RenderPass.hpp"
#include "RenderState.hpp"
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

	if (buffer->getUsage() == IDataBuffer::Usage::UniformBuffer) {
		VulkanBuffer* vulkanBuffer{ dynamic_cast<VulkanBuffer*>(buffer.get()) };
		uniformBufferInfo.buffer = vulkanBuffer->getVkBuffer();
		uniformBufferInfo.offset = 0;
		uniformBufferInfo.range = vulkanBuffer->length();
	}
	else { //  IDataBuffer::Usage::VertexBuffer
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

	if (uniformBufferInfo.buffer && vkRenderPass->updateDescriptorSets()) { // TODO: refector. This should be done in RenderState::PrepareImpl but the descriptor sets are not allocated
		// until RenderState::ApplyImpl.
		for (VkDescriptorSet& descriptor : vkRenderPass->descriptorSet()) {
			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptor;
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;

			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;

			descriptorWrite.pBufferInfo = &uniformBufferInfo;
			descriptorWrite.pImageInfo = nullptr; // Optional
			descriptorWrite.pTexelBufferView = nullptr; // Optional

			vkUpdateDescriptorSets(vkRenderState->getDevice(), 1, &descriptorWrite, 0, nullptr);
		}
	}

	vkCmdBindPipeline( vkRenderPass->commandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vkRenderState->getPipeline() );

	vkCmdBindDescriptorSets( vkRenderPass->commandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vkRenderState->getPipelineState().layout, 0, 1, vkRenderPass->descriptor(), 0, nullptr);

	uint32_t numBuffers{ static_cast<uint32_t>(dataBuffers.size()) };
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceSize> offsets;
	vertexBuffers.reserve(numBuffers);
	offsets.reserve(numBuffers);

	// TODO: Maybe one loop for all buffers and switch on type?
	for (auto& buffer : dataBuffers) {
		if (buffer->getUsage() != IDataBuffer::Usage::VertexBuffer)
			continue;
		VulkanBuffer* vulkanBuffer{ dynamic_cast<VulkanBuffer*>(buffer.get()) };
		vertexBuffers.push_back(vulkanBuffer->getVkBuffer());
		offsets.push_back( 0 );
	}

	if( !vertexBuffers.empty() )
		vkCmdBindVertexBuffers(vkRenderPass->commandBuffer(), 0, numBuffers, vertexBuffers.data(), offsets.data());

	if( instanceCount > 0 )
		vkCmdDraw( vkRenderPass->commandBuffer(), vertexCount, instanceCount, 0, 0 );
}

__declspec(dllexport)
	std::shared_ptr<IRenderCommand> CreateRenderCommand() {
		return std::make_shared<VulkanRenderCommand>();
	}