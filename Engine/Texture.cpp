//
//  Texture.cpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 2/20/19.
//

#include "Texture.hpp"
#include "Module.hpp"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT(IRenderTarget)

std::shared_ptr<ITexture> ITexture::Create( const glm::uvec2& dim, Format format ) {
   return ModuleFactory<RendererFactory>::Instance()->createTexture( dim, format );
}

std::shared_ptr<IRenderTarget> IRenderTarget::Create( const glm::uvec2& dim, Format format, Type type, Resource resource ) {
   return ModuleFactory<RendererFactory>::Instance()->createRenderTarget( dim, format, type, resource );
}

namespace boost { namespace serialization {
   template<class Archive> inline void serialize( Archive & ar, IRenderTarget& t, const unsigned int file_version ) {
      
   }
   
   template<class Archive> inline void save_construct_data( Archive & ar, const IRenderTarget* t, const unsigned int file_version ) {
      // save data required to construct instance
      ar << t->type;
      ar << t->resource;
      ar << t->format;
      ar << t->dim;
   }
   
   template<class Archive> inline void load_construct_data( Archive & ar, IRenderTarget* t, const unsigned int file_version ) {
      // retrieve data from archive required to construct new instance
      IRenderTarget::Type type;
      IRenderTarget::Resource resource;
      ITexture::Format format;
      glm::uvec2 dim;
      
      ar >> type;
      ar >> resource;
      ar >> format;
      ar >> dim;
      
      t = IRenderTarget::Create( dim, format, type, resource ).get();
   }
}}
