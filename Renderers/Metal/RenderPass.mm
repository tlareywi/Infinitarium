//
//
//

#include "RenderPass.h"
#include "RenderContext.h"

void MetalRenderPass::prepare( IRenderContext& renderContext ) {   
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

void MetalTexture::prepare( IRenderContext& renderContext ) {
   MetalRenderContext* context = dynamic_cast<MetalRenderContext*>( &renderContext );
   
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
      case RGBA8_sRGB:
         textureDescriptor.pixelFormat = MTLPixelFormatRGBA8Unorm_sRGB;
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
   textureDescriptor.storageMode = MTLStorageModeManaged;
   textureDescriptor.allowGPUOptimizedContents = false;
   textureDescriptor.usage = MTLTextureUsageShaderRead;
   
   // Stage to temporary CPU texture for copy. Strip bitmap header.
   std::uint8_t* data = std::get<DataPack_UINT8>(image).get();
   std::uint32_t offset = *reinterpret_cast<uint32_t*>(&data[10]);
   
   id<MTLTexture> tmpTexture = [context->getMTLDevice() newTextureWithDescriptor:textureDescriptor];
   [tmpTexture replaceRegion:MTLRegionMake2D(0, 0, dim.x, dim.y) mipmapLevel:0 slice:0 withBytes:(data + offset) bytesPerRow:(dim.x*bypp) bytesPerImage:0];
   
   // Explicit copy to GPU
   textureDescriptor.storageMode = MTLStorageModePrivate;
   textureDescriptor.allowGPUOptimizedContents = true;
   texture = [context->getMTLDevice() newTextureWithDescriptor:textureDescriptor];
   id<MTLCommandBuffer> cmdBuf = [context->getMTLCommandQ() commandBuffer];
   id<MTLBlitCommandEncoder> bltEncoder = [cmdBuf blitCommandEncoder];
   [bltEncoder copyFromTexture:tmpTexture
                   sourceSlice:0
                   sourceLevel:0
                   sourceOrigin:MTLOriginMake(0, 0, 0)
                   sourceSize:MTLSizeMake(dim.x, dim.y, 1)
                   toTexture:texture
                   destinationSlice:0
                   destinationLevel:0
                   destinationOrigin:MTLOriginMake(0, 0, 0)];
   [bltEncoder endEncoding];
   [cmdBuf commit];
   [cmdBuf waitUntilCompleted];
   
   [textureDescriptor release];
   [tmpTexture release];
}

MetalRenderTarget::MetalRenderTarget( const glm::uvec2& d, ITexture::Format f, IRenderTarget::Type t, IRenderTarget::Resource r ) :
   IRenderTarget( d, f, t, r ) {
      bytesPerRow = 0;
}

void MetalRenderTarget::prepare( IRenderContext& renderContext ) {
   MetalRenderContext* context = dynamic_cast<MetalRenderContext*>( &renderContext );
   
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
   // TODO: fix hardcoded bytes per pixel
   bytesPerRow = 4 * dim.x;
   bpp = 4;
   
   [textureDescriptor release];
   
   // Initialize buffer for GPU->CPU copies
   copyBuffer = std::make_unique<MetalDataBuffer>( renderContext );
}

void MetalRenderTarget::getData( const glm::uvec4& rect, void* data ) {
   copyBuffer->copy( *this, rect );
   copyBuffer->getData( data );
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
