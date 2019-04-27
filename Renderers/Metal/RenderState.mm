//
//  RenderState.m
//  IERenderer
//
//  Created by Trystan Larey-Williams on 12/31/18.
//

#include "RenderState.h"
#include "RenderProgram.h"

MetalRenderState::MetalRenderState() {
   renderDescriptor = [MTLRenderPipelineDescriptor new];
   [renderDescriptor reset];
   renderState = nullptr;
   currentDevice = nullptr;
}

MetalRenderState::~MetalRenderState() {
   [renderState release];
   [renderDescriptor release];
}

void MetalRenderState::prepareImpl( IRenderContext& context ) {
   MetalRenderContext* c = dynamic_cast<MetalRenderContext*>( &context );
   currentDevice = c->getMTLDevice();
   
   sanityCheck( currentDevice, context );
   
   // TODO: Temporary. Need to expose setters and serialize blend state.
   MTLRenderPipelineColorAttachmentDescriptor* attachement = renderDescriptor.colorAttachments[0];
   attachement.blendingEnabled = YES;
   attachement.rgbBlendOperation = MTLBlendOperationAdd;
   attachement.alphaBlendOperation = MTLBlendOperationAdd;
   attachement.sourceRGBBlendFactor = MTLBlendFactorOne;
   attachement.sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
   attachement.destinationRGBBlendFactor = MTLBlendFactorOne;
   attachement.destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
   
   [renderState release];
   renderState = nullptr;
}

void MetalRenderState::applyImpl( IRenderPass& renderPass ) {
   if( renderState || !currentDevice ) return;
   
   MetalRenderPass& metalRenderPass = dynamic_cast<MetalRenderPass&>( renderPass );
   MTLRenderPassDescriptor* desc = metalRenderPass.getPassDescriptor();
   unsigned int i = 0;
   MTLRenderPassColorAttachmentDescriptor* obj = nil;
   while( i < 8 ) {
      obj = desc.colorAttachments[i];
      if( obj == nil ) break;
      if( obj.texture )
         renderDescriptor.colorAttachments[i].pixelFormat = obj.texture.pixelFormat;
      else
         renderDescriptor.colorAttachments[i].pixelFormat = MTLPixelFormatInvalid;
      
      ++i;
   }
   
   NSError* err {nullptr};
   renderState = [currentDevice newRenderPipelineStateWithDescriptor:renderDescriptor error:&err];
   if( err ) {
      std::cout<<"Error creating PipelineState. "<<[err.localizedDescription UTF8String]<<std::endl;
   }
}

void MetalRenderState::sanityCheck( id<MTLDevice> device, IRenderContext& context ) { // Absolute minimal pipline specification to not SIGABRT on pipline state creation.
   if( !renderDescriptor.vertexFunction ) {
      MetalRenderProgram program;
      program.compile( "default", context );
      program.prepare( *this );
   }
}

MTLRenderPipelineDescriptor* MetalRenderState::getPipelineDescriptor() {
   return renderDescriptor;
}

id<MTLRenderPipelineState> MetalRenderState::getPipelineState() const {
   return renderState;
}

extern "C" {
   std::shared_ptr<IRenderState> CreateRenderState() {
      return std::make_shared<MetalRenderState>();
   }
}
