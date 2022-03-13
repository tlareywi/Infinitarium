//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "Texture.hpp"
#include "Module.hpp"

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_IMPLEMENT(BlendState)
BOOST_CLASS_EXPORT_IMPLEMENT(TextureProxy)
BOOST_CLASS_TRACKING(TextureProxy, boost::serialization::track_never)

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

static std::map<unsigned long long, std::shared_ptr<ITexture>> registeredObjs = std::map<unsigned long long, std::shared_ptr<ITexture>>();

std::shared_ptr<ITexture> ITexture::Create( unsigned int x, unsigned int y, Format format ) {
   return ModuleFactory<RendererFactory>::Instance()->createTexture( glm::uvec2(x,y), format );
}

std::shared_ptr<ITexture> ITexture::Clone( const ITexture& obj ) {
	std::map<unsigned long long, std::shared_ptr<ITexture>>::iterator it{ registeredObjs.find(obj._objId) };

	if (it == registeredObjs.end())
		registeredObjs[obj._objId] = ModuleFactory<RendererFactory>::Instance()->cloneTexture(obj);

	return registeredObjs[obj._objId];
}

void ITexture::clearRegisteredObjs() {
	registeredObjs.clear();
}

template<class Archive> void TextureProxy::serialize( Archive& ar, const unsigned int version ) {
	ar & BOOST_SERIALIZATION_NVP(dim);
	ar & BOOST_SERIALIZATION_NVP(format);
	ar & BOOST_SERIALIZATION_NVP(image);
	ar & BOOST_SERIALIZATION_NVP(_objId);
	std::cout << "Serializing Texture " << dim.x << " x " << dim.y << std::endl;
}
