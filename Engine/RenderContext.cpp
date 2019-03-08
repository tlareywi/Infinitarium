//
//  RenderContext.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/10/18.
//

#include "RenderContext.hpp"
#include "Module.hpp"

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(RenderContextProxy)

std::shared_ptr<IRenderContext> IRenderContext::Create( unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs ) {
   return ModuleFactory<RendererFactory>::Instance()->createRenderContext(x, y, w, h, fs);
}

std::shared_ptr<IRenderContext> IRenderContext::Clone( const IRenderContext& obj ) {
   return ModuleFactory<RendererFactory>::Instance()->cloneRenderContext(obj);
}

namespace boost { namespace serialization {
   template<class Archive> inline void serialize(Archive& ar, RenderContextProxy& t, unsigned int version) {
      std::cout<<"Serializing RenderContextProxy"<<std::endl;
      ar & t._x;
      ar & t._y;
      ar & t._width;
      ar & t._height;
      ar & t._fullScreen;
   }
}}

