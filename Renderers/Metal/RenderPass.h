#pragma once

#include "../../Engine/RenderPass.hpp"
#include "../../Engine/Texture.hpp"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

///
/// \brief Metal implementation of Texture
///
class MetalTexture : public ITexture {
public:
   MetalTexture( const glm::uvec2&, Format );
   
   void prepare( std::shared_ptr<IRenderContext>& ) override;
};

///
/// \brief Metal implementation of RenderTarget
///
class MetalRenderTarget : public IRenderTarget {
public:
   MetalRenderTarget( const IRenderTarget& obj ) : IRenderTarget(obj) {}
   MetalRenderTarget( const glm::uvec2&, Format, Type, Resource );
   
   id<MTLTexture> getMetalTexture() {
      return renderTarget;
   }
   
   void prepare( std::shared_ptr<IRenderContext>& ) override;
   
private:
   id<MTLTexture> renderTarget = nullptr;
};

///
/// \brief Metal implementation of RendePass
///
class MetalRenderPass : public IRenderPass {
public:
   MetalRenderPass() {}
   MetalRenderPass(const IRenderPass& obj) : IRenderPass(obj) {}
   void prepare( std::shared_ptr<IRenderContext>& ) override;
   void begin( std::shared_ptr<IRenderContext>& ) override;
   void end() override;
   
   id<MTLCommandBuffer> getCommandBuffer();
   MTLRenderPassDescriptor* getPassDescriptor();
   
private:
   MTLRenderPassDescriptor* renderPass;
   id<CAMetalDrawable> drawable;
   id <MTLCommandBuffer> commandBuffer;
};
