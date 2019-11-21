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
   { MTLRenderPipelineColorAttachmentDescriptor* attachement = renderDescriptor.colorAttachments[0];
   attachement.blendingEnabled = YES;
   attachement.rgbBlendOperation = MTLBlendOperationAdd;
   attachement.alphaBlendOperation = MTLBlendOperationAdd;
   attachement.sourceRGBBlendFactor = MTLBlendFactorOne;
   attachement.sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
   attachement.destinationRGBBlendFactor = MTLBlendFactorOne;
   attachement.destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha; }
   
   { MTLRenderPipelineColorAttachmentDescriptor* attachement = renderDescriptor.colorAttachments[1];
      attachement.blendingEnabled = YES;
      attachement.rgbBlendOperation = MTLBlendOperationMax;
      attachement.alphaBlendOperation = MTLBlendOperationMax;
      attachement.sourceRGBBlendFactor = MTLBlendFactorOne;
      attachement.sourceAlphaBlendFactor = MTLBlendFactorOne;
      attachement.destinationRGBBlendFactor = MTLBlendFactorOne;
      attachement.destinationAlphaBlendFactor = MTLBlendFactorOne; }
   
   [renderState release];
   renderState = nullptr;
}

void MetalRenderState::applyImpl( IRenderPass& renderPass ) {
   if( renderState || !currentDevice ) return;
   
   unsigned int i = 0;
   for( const auto& attachment : renderPass.getRenderTargets() )
      const MetalRenderTarget& target = dynamic_cast<const MetalRenderTarget&>( attachment );
      renderDescriptor.colorAttachments[i++].pixelFormat = target.getPixelFormat();
      
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

std::shared_ptr<IRenderState> CreateRenderState() {
   return std::make_shared<MetalRenderState>();
}

