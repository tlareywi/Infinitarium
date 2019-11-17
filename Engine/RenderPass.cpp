//
//  RenderPass.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#include "RenderPass.hpp"
#include "Module.hpp"

#include <boost/archive/polymorphic_xml_woarchive.hpp>
#include <boost/archive/polymorphic_xml_wiarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(RenderPassProxy)

std::shared_ptr<IRenderPass> IRenderPass::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createRenderPass();
}

std::shared_ptr<IRenderPass> IRenderPass::Clone( const IRenderPass& rp ) {
   return ModuleFactory<RendererFactory>::Instance()->createRenderPassCopy( rp );
}

template<class Archive> void RenderPassProxy::load( Archive& ar, const unsigned int version ) {
   std::cout << "Serializing RenderPassProxy" << std::endl;
   size_t sz;
   ar >> BOOST_SERIALIZATION_NVP(sz);
   if( sz > 0 ) {
      std::vector<std::shared_ptr<RenderTargetProxy>> baseTargets;
      ar >> BOOST_SERIALIZATION_NVP(baseTargets);
      targets.reserve(baseTargets.size());
      for( auto& t : baseTargets ) {
         targets.push_back( IRenderTarget::Clone(*t) );
      }
   }
}

template<class Archive> void RenderPassProxy::save( Archive& ar, const unsigned int version ) const {
   std::cout << "Serializing RenderPassProxy" << std::endl;
   // We need to serialize a 'proxy' class in order to maintain platform independent
   // scene files. When we desrialize the proxy class, we use the factory method
   // to re-instantiate a platform specific implementation.
   size_t sz {targets.size()};
   ar << BOOST_SERIALIZATION_NVP(sz);
   
   std::vector<std::shared_ptr<RenderTargetProxy>> baseTargets;
   baseTargets.reserve( sz );
   for( auto& t : targets )
      baseTargets.push_back( std::make_shared<RenderTargetProxy>(*t) );
   
   if( sz )
      ar << BOOST_SERIALIZATION_NVP(baseTargets);
}







