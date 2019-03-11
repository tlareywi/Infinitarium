//
//  Texture.cpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 2/20/19.
//

#include "Texture.hpp"
#include "Module.hpp"

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_IMPLEMENT(RenderTargetProxy)

////////////////////////////////////////////////////////////////////////////////////////
// ITexture
////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ITexture> ITexture::Create( const glm::uvec2& dim, Format format ) {
   return ModuleFactory<RendererFactory>::Instance()->createTexture( dim, format );
}

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
