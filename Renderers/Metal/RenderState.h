#pragma once

#include "../../Engine/RenderState.hpp"
#include "RenderContext.h"
#include "RenderPass.h"

#import <Metal/Metal.h>

class MetalRenderState : public IRenderState {
public:
   MetalRenderState();
   virtual ~MetalRenderState();
   
   void prepareImpl( IRenderContext& context, IRenderCommand& commmand ) override;
   void applyImpl( IRenderPass& ) override;
   
   void sanityCheck( id<MTLDevice> device, IRenderContext& context );
   
   MTLRenderPipelineDescriptor* getPipelineDescriptor();
   
   id<MTLRenderPipelineState> getPipelineState() const;
   
private:
   MTLRenderPipelineDescriptor* renderDescriptor;
   id<MTLRenderPipelineState> renderState;
   id<MTLDevice> currentDevice;
};
