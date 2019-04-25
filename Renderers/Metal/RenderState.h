#pragma once

#include "../../Engine/RenderState.hpp"
#include "RenderContext.h"
#include "RenderPass.h"

#import <Metal/Metal.h>

class MetalRenderState : public IRenderState {
public:
   MetalRenderState();
   virtual ~MetalRenderState();
   
   void commit( IRenderContext& context ) override;
   
   void sanityCheck( id<MTLDevice> device, IRenderContext& context );
   void resolveTargets( MetalRenderPass& );
   
   MTLRenderPipelineDescriptor* getPipelineDescriptor();
   
   id<MTLRenderPipelineState> getPipelineState() const;
   
private:
   MTLRenderPipelineDescriptor* renderDescriptor;
   id<MTLRenderPipelineState> renderState;
};
