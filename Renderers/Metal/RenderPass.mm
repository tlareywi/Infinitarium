//
//
//

#include "RenderPass.h"
#include "RenderContext.h"

void MetalRenderPass::begin() {
   MetalRenderContext* context = dynamic_cast<MetalRenderContext*>( renderContext.get() );
   CAMetalLayer* layer = context->getMTLLayer();
   drawable = [layer nextDrawable];
   if( !drawable ) return;
   
   commandBuffer = [context->getMTLCommandQ() commandBuffer];
   commandBuffer.label = @"IRenderPass";
   
   renderPass = [MTLRenderPassDescriptor renderPassDescriptor];
   renderPass.colorAttachments[0].texture = drawable.texture;
   renderPass.colorAttachments[0].loadAction = MTLLoadActionClear;
   renderPass.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);
   renderPass.colorAttachments[0].storeAction = MTLStoreActionStore;
}

void MetalRenderPass::end() {
   if( !drawable ) return;
   
   [commandBuffer presentDrawable:drawable];
   [commandBuffer commit];
   
   [commandBuffer release];
   [drawable release];
   [renderPass release];
}

id<MTLCommandBuffer> MetalRenderPass::getCommandBuffer() {
   return commandBuffer;
}

MTLRenderPassDescriptor* MetalRenderPass::getPassDescriptor() {
   return renderPass;
}

extern "C" {
   std::shared_ptr<IRenderPass> CreateRenderPass() {
      return std::make_shared<MetalRenderPass>();
   }
}
