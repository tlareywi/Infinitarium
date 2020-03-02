#include "RenderCommand.hpp"
#include "RenderPass.hpp"
#include "RenderState.hpp"
#include "DataBuffer.hpp"

#include <glm/gtc/matrix_transform.hpp>

template<typename T, typename U> const U static inline convert(const T& t) {
	U retType;
	memcpy(&retType, &t, sizeof(U));
	return retType;
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

/*	MetalRenderPass* metalRenderPass = dynamic_cast<MetalRenderPass*>(&renderPass);
		const MetalRenderState& metalRenderState = dynamic_cast<const MetalRenderState&>(state);
		if (!metalRenderPass) {
			std::cout << "WARNING, failed cast to MetalRenderCommand." << std::endl;
			return;
		}

		MTLRenderPassDescriptor* desc = metalRenderPass->getPassDescriptor();
		if (!desc)
			return;

		id<MTLCommandBuffer> cmdBuf = metalRenderPass->getCommandBuffer();
		id <MTLRenderCommandEncoder> commandEncoder = [cmdBuf renderCommandEncoderWithDescriptor : desc];

		commandEncoder.label = @"MyRenderEncoder";

			id<MTLRenderPipelineState> renderState = metalRenderState.getPipelineState();
		if (renderState)
			[commandEncoder setRenderPipelineState : renderState];

		{
			unsigned int indx{ 0 };
			for (auto& buffer : dataBuffers) {
				MetalDataBuffer* mtlBuf = dynamic_cast<MetalDataBuffer*>(buffer.get());
				if (!mtlBuf) continue;
				[commandEncoder setVertexBuffer : mtlBuf->getMTLBuffer() offset : 0 atIndex : indx++] ;
			}
		}

		{
			unsigned int indx{ 0 };
			for (auto& texture : textures) {
				MetalTexture* mtlTex = dynamic_cast<MetalTexture*>(texture.get());
				if (!mtlTex) continue;
				[commandEncoder setFragmentTexture : mtlTex->getMTLTexture() atIndex : indx] ;
			}
		}

		[commandEncoder drawPrimitives : primitiveType vertexStart : 0 vertexCount : vertexCount instanceCount : instanceCount baseInstance : 0];

		[commandEncoder endEncoding] ; */

	vkCmdBindPipeline( vkRenderPass->commandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vkRenderState->getPipeline() );

	//vkCmdDraw( vkRenderPass->commandBuffer(), 3, 1, 0, 0 );
}

__declspec(dllexport)
	std::shared_ptr<IRenderCommand> CreateRenderCommand() {
		return std::make_shared<VulkanRenderCommand>();
	}