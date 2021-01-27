//
//  RenderPass.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#include "RenderPass.hpp"
#include "Module.hpp"

#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_IMPLEMENT(RenderPassProxy)

static std::map<unsigned long long, std::shared_ptr<IRenderPass>> registeredObjs = std::map<unsigned long long, std::shared_ptr<IRenderPass>>();

std::shared_ptr<IRenderPass> IRenderPass::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createRenderPass();
}

std::shared_ptr<IRenderPass> IRenderPass::Clone( const IRenderPass& obj ) {
    std::map<unsigned long long, std::shared_ptr<IRenderPass>>::iterator it{ registeredObjs.find(obj._objId) };

    if (it == registeredObjs.end())
        registeredObjs[obj._objId] = ModuleFactory<RendererFactory>::Instance()->createRenderPassCopy(obj);

    return registeredObjs[obj._objId];
}

void IRenderPass::clearRegisteredObjs() {
    registeredObjs.clear();
}

template<class Archive> void RenderPassProxy::load( Archive& ar, const unsigned int version ) {
   ar >> _objId;
   ar >> loadOps;

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

   std::cout << "Serializing RenderPassProxy " << _objId << std::endl;
}

template<class Archive> void RenderPassProxy::save( Archive& ar, const unsigned int version ) const {
   std::cout << "Serializing RenderPassProxy " << _objId << std::endl;
   // We need to serialize a 'proxy' class in order to maintain platform independent
   // scene files. When we desrialize the proxy class, we use the factory method
   // to re-instantiate a platform specific implementation.
   ar << _objId;
   ar << loadOps;

   size_t sz {targets.size()};
   ar << BOOST_SERIALIZATION_NVP(sz);
   
   std::vector<std::shared_ptr<RenderTargetProxy>> baseTargets;
   baseTargets.reserve( sz );
   for( auto& t : targets )
      baseTargets.push_back( std::make_shared<RenderTargetProxy>(*t) );
   
   if( sz )
      ar << BOOST_SERIALIZATION_NVP(baseTargets);
}







