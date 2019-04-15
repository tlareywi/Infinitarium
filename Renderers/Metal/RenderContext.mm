//
//
//

#include "RenderContext.h"

MetalRenderContext::MetalRenderContext( unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs ) : IRenderContext(x, y, w, h, fs) {
   device = MTLCreateSystemDefaultDevice();
   surface = [[CAMetalLayer new] init];
   CGRect newFrame = CGRectMake( x, y, w, h );
      
   surface.pixelFormat = MTLPixelFormatBGRA8Unorm;
   surface.device = device;
   surface.framebufferOnly = YES;
   surface.frame = newFrame;
   surface.displaySyncEnabled = NO;
   surface.drawableSize = newFrame.size;
   
   commandQ = [device newCommandQueue];
}

MetalRenderContext::MetalRenderContext( const IRenderContext& obj ) : IRenderContext(obj) {
   device = MTLCreateSystemDefaultDevice();
   surface = [[CAMetalLayer new] init];
   CGRect newFrame = CGRectMake( _x, _y, _width, _height );
   
   surface.pixelFormat = MTLPixelFormatBGRA8Unorm;
   surface.device = device;
   surface.framebufferOnly = YES;
   surface.frame = newFrame;
   surface.displaySyncEnabled = NO;
   surface.drawableSize = newFrame.size;
   
   commandQ = [device newCommandQueue];
}

MetalRenderContext::~MetalRenderContext() {
   [commandQ release];
   [device release];
   [surface release];
}
   
id<MTLDevice> MetalRenderContext::getMTLDevice() {
   return device;
}
   
id<MTLCommandQueue> MetalRenderContext::getMTLCommandQ() {
   return commandQ;
}
   
CAMetalLayer* MetalRenderContext::getMTLLayer() {
   return surface;
}
   
CGRect MetalRenderContext::getCGRect() {
   return rect;
}
   
void* MetalRenderContext::getSurface() {
   return (__bridge void*)surface;
}

extern "C" {
   std::shared_ptr<IRenderContext> CreateRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs) {
      std::shared_ptr<IRenderContext> context = std::make_shared<MetalRenderContext>(x, y, w, h, fs);
      return context;
   }
}

extern "C" {
   std::shared_ptr<IRenderContext> CloneRenderContext( const IRenderContext& obj ) {
      std::shared_ptr<IRenderContext> context = std::make_shared<MetalRenderContext>(obj);
      return context;
   }
}
