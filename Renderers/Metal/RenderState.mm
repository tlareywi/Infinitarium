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
}

MetalRenderState::~MetalRenderState() {
   [renderState release];
   [renderDescriptor release];
}

void MetalRenderState::commit( IRenderContext& context ) {
   MetalRenderContext* c = dynamic_cast<MetalRenderContext*>( &context );
   id<MTLDevice> device = c->getMTLDevice();
   
   sanityCheck( device, context );
   
   MTLRenderPipelineColorAttachmentDescriptor* attachement = renderDescriptor.colorAttachments[0];
   attachement.blendingEnabled = YES;
   attachement.rgbBlendOperation = MTLBlendOperationAdd;
   attachement.alphaBlendOperation = MTLBlendOperationAdd;
   attachement.sourceRGBBlendFactor = MTLBlendFactorOne;
   attachement.sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
   attachement.destinationRGBBlendFactor = MTLBlendFactorOne;
   attachement.destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
   
   NSError* err {nullptr};
   [renderState release];
   renderState = [device newRenderPipelineStateWithDescriptor:renderDescriptor error:&err];
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
   
   if( !renderDescriptor.colorAttachments[0].pixelFormat ) {
      renderDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
   }
   
   // TODO: Temporary. Generalize.
   if( !renderDescriptor.colorAttachments[1].pixelFormat ) {
      renderDescriptor.colorAttachments[1].pixelFormat = MTLPixelFormatR32Uint;
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
