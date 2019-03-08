//
//  RenderPass.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#include "RenderPass.hpp"
#include "Module.hpp"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(RenderPassProxy)

std::shared_ptr<IRenderPass> IRenderPass::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createRenderPass();
}

std::shared_ptr<IRenderPass> IRenderPass::CreateCopy( const IRenderPass& rp ) {
   return ModuleFactory<RendererFactory>::Instance()->createRenderPassCopy( rp );
}

template<class Archive> void RenderPassProxy::load( Archive& ar ) {
   size_t sz;
   ar >> sz;
   if( sz > 0 ) {
      std::vector<std::shared_ptr<RenderTargetProxy>> baseTargets;
      ar >> baseTargets;
      targets.reserve(baseTargets.size());
      for( auto& t : baseTargets ) {
         targets.push_back( IRenderTarget::CreateCopy(*t) );
      }
   }
}

template<class Archive> void RenderPassProxy::save( Archive& ar ) const {
   // We need to serialize a 'proxy' class in order to maintain platform independent
   // scene files. When we desrialize the proxy class, we use the factory method
   // to re-instantiate a platform specific implementation.
   size_t sz {targets.size()};
   ar << sz;
   
   std::vector<std::shared_ptr<RenderTargetProxy>> baseTargets;
   baseTargets.reserve( sz );
   for( auto& t : targets )
      baseTargets.push_back( std::make_shared<RenderTargetProxy>(*t) );
   
   if( sz )
      ar << baseTargets;
}

namespace boost { namespace serialization {
   template<class Archive> inline void load(Archive& ar, RenderPassProxy& t, unsigned int version) {
      std::cout<<"Loading RenderPassProxy"<<std::endl;
      t.load( ar );
   }
   template<class Archive> inline void save(Archive& ar, const RenderPassProxy& t, unsigned int version) {
      std::cout<<"Saving RenderPassProxy"<<std::endl;
      t.save( ar );
   }
   template<class Archive> inline void serialize(Archive& ar, RenderPassProxy& t, const unsigned int version) {
      boost::serialization::split_free(ar, t, version);
   }
}}






