//
//  RenderState.m
//  IERenderer
//
//  Created by Trystan Larey-Williams on 12/31/18.
//

#include "RenderState.h"
#include "RenderProgram.h"

static const std::map<BlendState::Op, MTLBlendOperation> BlendOp {
   {BlendState::Op::Add, MTLBlendOperationAdd},
   {BlendState::Op::Subtract, MTLBlendOperationSubtract},
   {BlendState::Op::ReverseSubtract, MTLBlendOperationReverseSubtract},
   {BlendState::Op::Min, MTLBlendOperationMin},
   {BlendState::Op::Max, MTLBlendOperationMax}
};
static const std::map<BlendState::Factor, MTLBlendFactor> BlendFactor {
   {BlendState::Factor::Zero, MTLBlendFactorZero},
   {BlendState::Factor::One, MTLBlendFactorOne},
   {BlendState::Factor::SourceColor, MTLBlendFactorSourceColor},
   {BlendState::Factor::OneMinusSourceColor, MTLBlendFactorOneMinusSourceColor},
   {BlendState::Factor::SourceAlpha, MTLBlendFactorSourceAlpha},
   {BlendState::Factor::OneMinusSourceAlpha, MTLBlendFactorOneMinusSourceAlpha},
   {BlendState::Factor::DestinationColor, MTLBlendFactorDestinationColor},
   {BlendState::Factor::OneMinusDestinationColor, MTLBlendFactorOneMinusDestinationColor},
   {BlendState::Factor::DestinationAlpha, MTLBlendFactorDestinationAlpha},
   {BlendState::Factor::OneMinusDestinationAlpha, MTLBlendFactorOneMinusDestinationAlpha},
   {BlendState::Factor::SourceAlphaSaturated, MTLBlendFactorSourceAlphaSaturated},
   {BlendState::Factor::BlendColor, MTLBlendFactorBlendColor},
   {BlendState::Factor::OneMinusBlendColor, MTLBlendFactorOneMinusBlendColor},
   {BlendState::Factor::BlendAlpha, MTLBlendFactorBlendAlpha},
   {BlendState::Factor::OneMinusBlendAlpha, MTLBlendFactorOneMinusBlendAlpha},
   {BlendState::Factor::Source1Color, MTLBlendFactorSource1Color},
   {BlendState::Factor::OneMinusSource1Color, MTLBlendFactorOneMinusSource1Color},
   {BlendState::Factor::Source1Alpha, MTLBlendFactorSource1Alpha},
   {BlendState::Factor::OneMinusSource1Alpha, MTLBlendFactorOneMinusSource1Alpha}
};

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

void MetalRenderState::prepareImpl( IRenderContext& context, IRenderCommand& commmand ) {
   MetalRenderContext* c = dynamic_cast<MetalRenderContext*>( &context );
   currentDevice = c->getMTLDevice();
   
   sanityCheck( currentDevice, context );
   
   [renderState release];
   renderState = nullptr;
}

void MetalRenderState::applyImpl( IRenderPass& renderPass ) {
   if( renderState || !currentDevice ) return;
   
   unsigned int i = 0;
   for( const auto& attachment : renderPass.getRenderTargets() ) {
      const MetalRenderTarget& target = dynamic_cast<const MetalRenderTarget&>( *(attachment.get()) );
      MTLRenderPipelineColorAttachmentDescriptor* attachement = renderDescriptor.colorAttachments[i++];
      const BlendState& blending {target.getBlendState()};
      attachement.pixelFormat = target.getPixelFormat();
      attachement.blendingEnabled = blending.enabled;
      attachement.rgbBlendOperation = BlendOp.find(blending.rgbBlendOperation)->second;
      attachement.alphaBlendOperation = BlendOp.find(blending.alphaBlendOperation)->second;
      attachement.sourceRGBBlendFactor = BlendFactor.find(blending.sourceRGB)->second;
      attachement.sourceAlphaBlendFactor = BlendFactor.find(blending.sourceAlpha)->second;;
      attachement.destinationRGBBlendFactor = BlendFactor.find(blending.destinationRGB)->second;;
      attachement.destinationAlphaBlendFactor = BlendFactor.find(blending.destinationAlpha)->second;;
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

