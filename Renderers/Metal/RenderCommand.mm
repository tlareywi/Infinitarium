#include "RenderCommand.h"
#include "RenderPass.h"
#include "RenderState.h"
#include "DataBuffer.h"

#include <glm/gtc/matrix_transform.hpp>

template<typename T, typename U> const U static inline convert( const T& t ) {
   U retType;
   memcpy( &retType, &t, sizeof(U) );
   return retType;
}

void MetalRenderCommand::setPrimitiveType( PrimitiveType t ) {
   switch( t ) {
      case Triangle:
         primitiveType = MTLPrimitiveTypePoint;
         break;
      
      case TriangleStrip:
         primitiveType = MTLPrimitiveTypeTriangleStrip;
         break;
      
      case Line:
         primitiveType = MTLPrimitiveTypeLine;
         break;
      
      case Point:
      default:
         primitiveType = MTLPrimitiveTypePoint;
         break;
   }
}

void MetalRenderCommand::encode( IRenderPass& renderPass, IRenderState& state ) {
   @autoreleasepool {
      MetalRenderPass* metalRenderPass = dynamic_cast<MetalRenderPass*>( &renderPass );
      MetalRenderState& metalRenderState = dynamic_cast<MetalRenderState&>( state );
      if( !metalRenderPass ) {
         std::cout<<"WARNING, failed cast to MetalRenderCommand."<<std::endl;
         return;
      }
      
      MTLRenderPassDescriptor* desc = metalRenderPass->getPassDescriptor();
      if( !desc )
         return;
      
      id<MTLCommandBuffer> cmdBuf = metalRenderPass->getCommandBuffer();
      id <MTLRenderCommandEncoder> commandEncoder = [cmdBuf renderCommandEncoderWithDescriptor:desc];
      
      commandEncoder.label = @"MyRenderEncoder";
      
      id<MTLRenderPipelineState> renderState = metalRenderState.getPipelineState();
      if( renderState )
         [commandEncoder setRenderPipelineState:renderState];
      
      {
         unsigned int indx {0};
         for( auto& buffer : dataBuffers ) {
            MetalDataBuffer* mtlBuf = dynamic_cast<MetalDataBuffer*>(buffer.get());
            if( !mtlBuf ) continue;
            [commandEncoder setVertexBuffer:mtlBuf->getMTLBuffer() offset:0 atIndex:indx++];
         }
      }
      
      {
         unsigned int indx {0};
         for( auto& texture : textures ) {
            MetalTexture* mtlTex = dynamic_cast<MetalTexture*>(texture.get());
            if( !mtlTex ) continue;
            [commandEncoder setFragmentTexture:mtlTex->getMTLTexture() atIndex:indx];
         }
      }
      
      [commandEncoder drawPrimitives:primitiveType vertexStart:0 vertexCount:vertexCount instanceCount:instanceCount baseInstance:0];
      
      [commandEncoder endEncoding];
   }
}

extern "C" {
   std::shared_ptr<IRenderCommand> CreateRenderCommand() {
      return std::make_shared<MetalRenderCommand>();
   }
}
