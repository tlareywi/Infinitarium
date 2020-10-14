//
//  Texture.cpp
//  IERenderer
//
//  Created by Trystan (Home) on 2/20/19.
//

#include "Texture.hpp"

MetalTexture::MetalTexture( glm::uvec2 dim, ITexture::Format format ) {
   MTLTextureDescriptor* texDesc = [[MTLTextureDescriptor alloc] init];
   switch( format ) {
      case ITexture::RU32:
         texDesc.pixelFormat = MTLPixelFormatR32Uint;
         break;
      case ITexture::BRGA8_sRGB:
         texDesc.pixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
         break;
      case BRGA8:
      default:
         texDesc.pixelFormat = MTLPixelFormatBGRA8Unorm;
         break;
   }
   
   texDesc.width = dim.x;
   texDesc.height = dim.y;
   
   
   
   [texDesc release];
}

extern "C" {
   std::shared_ptr<ITexture> CreateTexture( glm::uvec2 dim, ITexture::Format format ) {
      return std::make_shared<MetalTexture>( dim, format );
   }
}
