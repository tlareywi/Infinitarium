#pragma once

#include "../../Engine/RenderPass.hpp"
#include "../../Engine/Texture.hpp"

#include "DataBuffer.h"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

///
/// \brief Metal implementation of Texture
///
class MetalTexture : public ITexture {
public:
   MetalTexture( const glm::uvec2&, Format );
   MetalTexture( const ITexture& obj ) : ITexture(obj) {};
   virtual ~MetalTexture() {
      [texture release];
      texture = nullptr;
   }
   
   void prepare( IRenderContext& ) override;
   
   id<MTLTexture> getMTLTexture() {
      return texture;
   }
   
private:
   id<MTLTexture> texture = nullptr;
};

///
/// \brief Metal implementation of RenderTarget
///
class MetalRenderTarget : public IRenderTarget {
public:
   MetalRenderTarget( const IRenderTarget& obj ) : IRenderTarget(obj), bytesPerRow(0) {}
   MetalRenderTarget( const glm::uvec2&, Format, Type, Resource );
   virtual ~MetalRenderTarget() {
      [renderTarget release];
      renderTarget = nullptr;
   }
   
   void prepare( IRenderContext& ) override;
   void getData( const glm::uvec4&, void* ) override;
   
   id<MTLTexture> getMetalTexture() {
      return renderTarget;
   }
   
   unsigned int getBytesPerRow() {
      return bytesPerRow;
   }
   
   MTLPixelFormat getPixelFormat() const;
   
   unsigned short getBytesPerPixel() {
      return bpp;
   }
   
private:
   id<MTLTexture> renderTarget = nullptr;
   std::unique_ptr<MetalDataBuffer> copyBuffer;
   
   unsigned int bytesPerRow;
   unsigned short bpp;
};

///
/// \brief Metal implementation of RendePass
///
class MetalRenderPass : public IRenderPass {
public:
   MetalRenderPass() {}
   MetalRenderPass(const IRenderPass& obj) : IRenderPass(obj) {}
   void prepare( IRenderContext& ) override;
   void begin( std::shared_ptr<IRenderContext>& ) override;
   void end() override;
   
   id<MTLCommandBuffer> getCommandBuffer();
   MTLRenderPassDescriptor* getPassDescriptor();
   
private:
   MTLRenderPassDescriptor* renderPass = nullptr;
   id<CAMetalDrawable> drawable = nullptr;
   id <MTLCommandBuffer> commandBuffer = nullptr;
};
