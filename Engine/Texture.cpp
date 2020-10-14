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

template<class Archive> void BlendState::serialize(Archive& ar, unsigned int version) {
	std::cout<<"Serializing BlendState"<<std::endl;
    ar & BOOST_SERIALIZATION_NVP(enabled);
    ar & BOOST_SERIALIZATION_NVP(rgbBlendOperation);
    ar & BOOST_SERIALIZATION_NVP(alphaBlendOperation);
    ar & BOOST_SERIALIZATION_NVP(sourceRGB);
    ar & BOOST_SERIALIZATION_NVP(sourceAlpha);
    ar & BOOST_SERIALIZATION_NVP(destinationRGB);
    ar & BOOST_SERIALIZATION_NVP(destinationAlpha);
}

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
	ar & BOOST_SERIALIZATION_NVP(dim);
	ar & BOOST_SERIALIZATION_NVP(format);
	ar & BOOST_SERIALIZATION_NVP(image);
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
	ar & BOOST_SERIALIZATION_NVP(dim);
	ar & BOOST_SERIALIZATION_NVP(format);
	ar & BOOST_SERIALIZATION_NVP(type);
	ar & BOOST_SERIALIZATION_NVP(resource);
	ar & BOOST_SERIALIZATION_NVP(clear);
	ar & BOOST_SERIALIZATION_NVP(clearColor);
    ar & BOOST_SERIALIZATION_NVP(blending);
}

