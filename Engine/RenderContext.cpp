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
static std::map<unsigned long long, std::shared_ptr<IRenderContext>> registeredObjs = std::map<unsigned long long, std::shared_ptr<IRenderContext>>();

std::shared_ptr<IRenderContext> IRenderContext::Create( unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs, bool headset ) {

   return ModuleFactory<RendererFactory>::Instance()->createRenderContext(x, y, w, h, fs, headset);
}

std::shared_ptr<IRenderContext> IRenderContext::Clone( const IRenderContext& obj ) {
   std::map<unsigned long long, std::shared_ptr<IRenderContext>>::iterator it{ registeredObjs.find(obj._objId) };

   if (it == registeredObjs.end())
      registeredObjs[obj._objId] = ModuleFactory<RendererFactory>::Instance()->cloneRenderContext(obj);

   return registeredObjs[obj._objId];
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
	ar & BOOST_SERIALIZATION_NVP(_headset);
	ar & BOOST_SERIALIZATION_NVP(_objId);
	
	// TODO: Temporary override for dev/testing. For now, we'll just use the current video mode of the primary monitor always.
	// Also, fullscreen causes issues when step debugging on Visual Studio (unrecoverable hang). 
	_fullScreen = true;
}


