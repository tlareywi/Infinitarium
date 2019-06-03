#pragma once

#include "../../Engine/RenderContext.hpp"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

///
///
///
class MetalRenderContext : public IRenderContext {
public:
   MetalRenderContext( const IRenderContext& obj );
   MetalRenderContext( unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs );
   virtual ~MetalRenderContext();
   
   id<MTLDevice> getMTLDevice();
   
   id<MTLCommandQueue> getMTLCommandQ();
   
   CAMetalLayer* getMTLLayer();
   
   CGRect getCGRect();
   
   void* getSurface() override;
   void setSurface( void* ) override {}
   
private:
   id<MTLDevice> device;
   id<MTLCommandQueue> commandQ;
   CAMetalLayer* surface;
   CGRect rect;
};
