#pragma once

#include "../../Engine/RenderState.hpp"
#include "RenderContext.h"

#import <Metal/Metal.h>

class MetalRenderState : public IRenderState {
public:
   MetalRenderState();
   virtual ~MetalRenderState();
   
   void commit( IRenderContext& context ) override;
   
   void sanityCheck( id<MTLDevice> device, IRenderContext& context );
   
   MTLRenderPipelineDescriptor* getPipelineDescriptor();
   
   id<MTLRenderPipelineState> getPipelineState() const;
   
private:
   MTLRenderPipelineDescriptor* renderDescriptor;
   id<MTLRenderPipelineState> renderState;
};
