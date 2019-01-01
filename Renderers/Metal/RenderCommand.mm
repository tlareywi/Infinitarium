#include "RenderCommand.h"
#include "RenderPass.h"
#include "RenderState.h"
#include "DataBuffer.h"

#include <glm/gtc/matrix_transform.hpp>

#import <Metal/Metal.h>

template<typename T, typename U> const U static inline convert( const T& t ) {
   U retType;
   memcpy( &retType, &t, sizeof(U) );
   return retType;
}

void MetalRenderCommand::encode( IRenderPass& renderPass, const IRenderState& state ) {
   @autoreleasepool {
      MetalRenderPass* metalRenderPass = dynamic_cast<MetalRenderPass*>( &renderPass );
      const MetalRenderState& metalRenderState = dynamic_cast<const MetalRenderState&>( state );
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
      
      unsigned int indx {0};
      for( auto& buffer : dataBuffers ) {
         MetalDataBuffer* mtlBuf = dynamic_cast<MetalDataBuffer*>(buffer.get());
         [commandEncoder setVertexBuffer:mtlBuf->getMTLBuffer() offset:0 atIndex:indx++];
      }
      
      // TMP hack for testing
      [commandEncoder drawPrimitives:MTLPrimitiveTypePoint vertexStart:0 vertexCount:1 instanceCount:2539802 baseInstance:0];
      // end hack
      
      [commandEncoder endEncoding];
   }
}

extern "C" {
   std::shared_ptr<IRenderCommand> CreateRenderCommand() {
      return std::make_shared<MetalRenderCommand>();
   }
}
