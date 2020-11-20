//
//  RenderContext.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/10/18.
//

#include "RenderContext.hpp"
#include "Module.hpp"
#include "ApplicationWindow.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(RenderContextProxy)

static std::shared_ptr<IApplicationWindow> window = nullptr;

std::shared_ptr<IRenderContext> IRenderContext::Create( unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs ) {
   return ModuleFactory<RendererFactory>::Instance()->createRenderContext(x, y, w, h, fs);
}

std::shared_ptr<IRenderContext> IRenderContext::Clone( const IRenderContext& obj ) {
   return ModuleFactory<RendererFactory>::Instance()->cloneRenderContext(obj);
}

void IRenderContext::init() {
   if( _width && _height ) {
      window = IApplicationWindow::Create();
      window->init( *this );
   }
}

template<class Archive> void RenderContextProxy::serialize(Archive& ar, const unsigned int version) {
	std::cout << "Serializing RenderContextProxy" << std::endl;
	ar & BOOST_SERIALIZATION_NVP(_x);
	ar & BOOST_SERIALIZATION_NVP(_y);
	ar & BOOST_SERIALIZATION_NVP(_width);
	ar & BOOST_SERIALIZATION_NVP(_height);
	ar & BOOST_SERIALIZATION_NVP(_fullScreen);
	
	// TODO: Temporary override for dev/testing. For now, we'll just use the current video mode of the primary monitor always.
	//_fullScreen = true;
}


