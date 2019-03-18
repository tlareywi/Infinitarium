//
//
//

#include "RenderPass.h"
#include "RenderContext.h"

void MetalRenderPass::prepare( std::shared_ptr<IRenderContext>& renderContext ) {   
   for( auto& target : targets )
      target->prepare( renderContext );
}

void MetalRenderPass::begin( std::shared_ptr<IRenderContext>& renderContext ) {
   MetalRenderContext* context = dynamic_cast<MetalRenderContext*>( renderContext.get() );
   commandBuffer = [context->getMTLCommandQ() commandBuffer];
   commandBuffer.label = @"IRenderPass";
   
   renderPass = [MTLRenderPassDescriptor renderPassDescriptor];
   
   unsigned short i {0};

   for( auto& target : targets ) {
      if( target->getResource() == IRenderTarget::FrameBuffer ) {
         CAMetalLayer* layer = context->getMTLLayer();
         drawable = [layer nextDrawable];
         if( !drawable ) return;
         
         renderPass.colorAttachments[i].texture = drawable.texture;
      }
      else if( target->getResource() == IRenderTarget::Memory ) {
         MetalRenderTarget* metalTarget = dynamic_cast<MetalRenderTarget*>( target.get() );
         renderPass.colorAttachments[i].texture = metalTarget->getMetalTexture();
      }
      
      if( target->getClear() ) {
         glm::vec4 color = target->getClearColor();
         renderPass.colorAttachments[i].clearColor = MTLClearColorMake(color.r, color.g, color.b, color.a);
         renderPass.colorAttachments[i].loadAction = MTLLoadActionClear;
      }
      else
         renderPass.colorAttachments[i].loadAction = MTLLoadActionLoad;
      
      renderPass.colorAttachments[i].storeAction = MTLStoreActionStore;
      ++i;
   }
}

void MetalRenderPass::end() {
   if( !drawable ) return;
   
   [commandBuffer presentDrawable:drawable];
   [commandBuffer commit];
   
   [commandBuffer release];
   [drawable release];
   [renderPass release];
}

id<MTLCommandBuffer> MetalRenderPass::getCommandBuffer() {
   return commandBuffer;
}

MTLRenderPassDescriptor* MetalRenderPass::getPassDescriptor() {
   return renderPass;
}

extern "C" {
   std::shared_ptr<IRenderPass> CreateRenderPass() {
      return std::make_shared<MetalRenderPass>();
   }
}

extern "C" {
   std::shared_ptr<IRenderPass> CreateRenderPassCopy( const IRenderPass& rp ) {
      return std::make_shared<MetalRenderPass>( rp );
   }
}

MetalTexture::MetalTexture( const glm::uvec2& d, ITexture::Format f ) : ITexture(d, f) {}

void MetalTexture::prepare( std::shared_ptr<IRenderContext>& renderContext ) {
   MetalRenderContext* context = dynamic_cast<MetalRenderContext*>( renderContext.get() );
   
   [texture release];
   
   MTLTextureDescriptor* textureDescriptor = [[MTLTextureDescriptor alloc] init];
   
   unsigned short bypp{4};
   switch( format ) {
      case BRGA8:
         textureDescriptor.pixelFormat = MTLPixelFormatBGRA8Unorm;
         break;
      case RU32:
         textureDescriptor.pixelFormat = MTLPixelFormatR32Uint;
         break;
      case RGBA8:
         textureDescriptor.pixelFormat = MTLPixelFormatRGBA8Unorm;
         break;
      case BRGA8_sRGB:
      default:
         textureDescriptor.pixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
         break;
   }
   
   textureDescriptor.width = dim.x;
   textureDescriptor.height = dim.y;
   textureDescriptor.depth = 1;
   textureDescriptor.mipmapLevelCount = 1;
   textureDescriptor.sampleCount = 1;
   textureDescriptor.arrayLength = 1;
   textureDescriptor.storageMode = MTLStorageModePrivate;
   textureDescriptor.allowGPUOptimizedContents = true;
   textureDescriptor.usage = MTLTextureUsageShaderRead;
   
   texture = [context->getMTLDevice() newTextureWithDescriptor:textureDescriptor];
   
   [textureDescriptor release];
   
   std::visit( [bypp, this](auto& e) {
      [texture replaceRegion:MTLRegionMake2D(0, 0, dim.x, dim.y) mipmapLevel:0 withBytes:e.get() bytesPerRow:(dim.x*bypp)];
   }, image );
}

MetalRenderTarget::MetalRenderTarget( const glm::uvec2& d, ITexture::Format f, IRenderTarget::Type t, IRenderTarget::Resource r ) :
   IRenderTarget( d, f, t, r ) {
}

void MetalRenderTarget::prepare( std::shared_ptr<IRenderContext>& renderContext ) {
   MetalRenderContext* context = dynamic_cast<MetalRenderContext*>( renderContext.get() );
   
   [renderTarget release];
   
   MTLTextureDescriptor* textureDescriptor = [[MTLTextureDescriptor alloc] init];
   
   switch( format ) {
      case BRGA8:
         textureDescriptor.pixelFormat = MTLPixelFormatBGRA8Unorm;
         break;
      case RU32:
         textureDescriptor.pixelFormat = MTLPixelFormatR32Uint;
         break;
      case BRGA8_sRGB:
      default:
         textureDescriptor.pixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
         break;
   }
   
   textureDescriptor.width = dim.x;
   textureDescriptor.height = dim.y;
   textureDescriptor.depth = 1;
   textureDescriptor.mipmapLevelCount = 1;
   textureDescriptor.sampleCount = 1;
   textureDescriptor.arrayLength = 1;
   textureDescriptor.storageMode = MTLStorageModePrivate;
   textureDescriptor.allowGPUOptimizedContents = true;
   textureDescriptor.usage = MTLTextureUsageRenderTarget;
   
   renderTarget = [context->getMTLDevice() newTextureWithDescriptor:textureDescriptor];
   
   [textureDescriptor release];
}

extern "C" {
   std::shared_ptr<IRenderTarget> CreateRenderTarget( const glm::uvec2& vec, ITexture::Format f, IRenderTarget::Type t, IRenderTarget::Resource r ) {
      return std::make_shared<MetalRenderTarget>( vec, f, t, r );
   }
}

extern "C" {
   std::shared_ptr<IRenderTarget> CloneRenderTarget( const IRenderTarget& obj ) {
      return std::make_shared<MetalRenderTarget>( obj );
   }
}

extern "C" {
   std::shared_ptr<ITexture> CreateTexture( const glm::uvec2& vec, ITexture::Format f  ) {
      return std::make_shared<MetalTexture>( vec, f );
   }
}

extern "C" {
   std::shared_ptr<ITexture> CloneTexture( const ITexture& obj ) {
      return std::make_shared<MetalTexture>( obj );
   }
}
