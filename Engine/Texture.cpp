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
BOOST_CLASS_EXPORT_IMPLEMENT(TextureProxy)

////////////////////////////////////////////////////////////////////////////////////////
// ITexture
////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ITexture> ITexture::Create( unsigned int x, unsigned int y, Format format ) {
   return ModuleFactory<RendererFactory>::Instance()->createTexture( glm::uvec2(x,y), format );
}

std::shared_ptr<ITexture> ITexture::Clone( const ITexture& obj ) {
   return ModuleFactory<RendererFactory>::Instance()->cloneTexture( obj );
}

template<class Archive> void TextureProxy::serialize( Archive& ar, const unsigned int version ) {
	std::cout << "Serializing Texture" << std::endl;
	ar & dim;
	ar & format;
	ar & image;
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

template<class Archive> void RenderTargetProxy::serialize( Archive& ar, const unsigned int version ) {
	std::cout << "Serializing RenderTargetProxy" << std::endl;
	ar & dim;
	ar & format;
	ar & type;
	ar & resource;
	ar & clear;
	ar & clearColor;
}

