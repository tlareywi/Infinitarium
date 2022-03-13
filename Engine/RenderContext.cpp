//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "RenderContext.hpp"
#include "Module.hpp"
#include "ApplicationWindow.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(RenderContextProxy)
BOOST_CLASS_TRACKING(RenderContextProxy, boost::serialization::track_never)

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

	  // Pick buffer at context scope. Many renderables will use this.
      _pickBuffer = IDataBuffer::Create(*this);
	  _pickBuffer->setUsage(IDataBuffer::Usage::Pick);
	  _pickBuffer->reserve(_width * _height * sizeof(PickUnit)); // Each pixel for pick buffer is a float + uint, so 8 bytes. 4K res uses ~66MB.
	  _pickBuffer->commit();

	  // Light map
	  _postProcBuffer = IDataBuffer::Create(*this);
	  _postProcBuffer->setUsage(IDataBuffer::Usage::PostProcess);
	  _postProcBuffer->reserve(_width * _height * sizeof(PostProcessUnit));
	  _postProcBuffer->commit();
   }
}

void IRenderContext::clearRegisteredObjs() {
	registeredObjs.clear();
}

template<class Archive> void RenderContextProxy::serialize(Archive& ar, const unsigned int version) {
	ar & BOOST_SERIALIZATION_NVP(_x);
	ar & BOOST_SERIALIZATION_NVP(_y);
	ar & BOOST_SERIALIZATION_NVP(_width);
	ar & BOOST_SERIALIZATION_NVP(_height);
	ar & BOOST_SERIALIZATION_NVP(_fullScreen);
	ar & BOOST_SERIALIZATION_NVP(_headset);
	ar & BOOST_SERIALIZATION_NVP(_objId);

	//_fullScreen = true;
	std::cout << "Serializing RenderContextProxy " << _objId << std::endl;
}


