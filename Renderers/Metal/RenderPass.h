#pragma once

#include "../../Engine/RenderPass.hpp"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

///
/// brief Metal implementation of RendePass
///
class MetalRenderPass : public IRenderPass {
public:
   void begin() override;
   void end() override;
   
   id<MTLCommandBuffer> getCommandBuffer();
   MTLRenderPassDescriptor* getPassDescriptor();
   
private:
   MTLRenderPassDescriptor* renderPass;
   id<CAMetalDrawable> drawable;
   id <MTLCommandBuffer> commandBuffer;
};
