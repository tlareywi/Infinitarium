//
//  Texture.cpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 2/20/19.
//

#include "Texture.hpp"
#include "Module.hpp"

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_IMPLEMENT(BlendState)
BOOST_CLASS_EXPORT_IMPLEMENT(RenderTargetProxy)
BOOST_CLASS_EXPORT_IMPLEMENT(TextureProxy)

namespace boost { namespace serialization {
   template<class Archive> inline void serialize(Archive& ar, BlendState& t, unsigned int version) {
      std::cout<<"Serializing BlandState"<<std::endl;
      ar & t.enabled;
      ar & t.rgbBlendOperation;
      ar & t.alphaBlendOperation;
      ar & t.sourceRGB;
      ar & t.sourceAlpha;
      ar & t.destinationRGB;
      ar & t.destinationAlpha;
   }
}}

////////////////////////////////////////////////////////////////////////////////////////
// ITexture
////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ITexture> ITexture::Create( unsigned int x, unsigned int y, Format format ) {
   return ModuleFactory<RendererFactory>::Instance()->createTexture( glm::uvec2(x,y), format );
}

std::shared_ptr<ITexture> ITexture::Clone( const ITexture& obj ) {
   return ModuleFactory<RendererFactory>::Instance()->cloneTexture( obj );
}

namespace boost { namespace serialization {
   template<class Archive> inline void serialize(Archive& ar, TextureProxy& t, unsigned int version) {
      std::cout<<"Serializing Texture"<<std::endl;
      ar & t.dim;
      ar & t.format;
      ar & t.image;
   }
}}

////////////////////////////////////////////////////////////////////////////////////////
// IRenderTarget
////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<IRenderTarget> IRenderTarget::Create( unsigned int x, unsigned int y, Format format, Type type, Resource resource ) {
   return ModuleFactory<RendererFactory>::Instance()->createRenderTarget( glm::uvec2(x,y), format, type, resource );
}

std::shared_ptr<IRenderTarget> IRenderTarget::Clone( const IRenderTarget& obj ) {
   return ModuleFactory<RendererFactory>::Instance()->createRenderTargetCopy( obj );
}

namespace boost { namespace serialization {
   template<class Archive> inline void serialize(Archive& ar, RenderTargetProxy& t, unsigned int version) {
      std::cout<<"Serializing RenderTargetProxy"<<std::endl;
      ar & t.dim;
      ar & t.format;
      ar & t.type;
      ar & t.resource;
      ar & t.clear;
      ar & t.clearColor;
   }
}}
