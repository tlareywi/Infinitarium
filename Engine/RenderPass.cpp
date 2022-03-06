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
BOOST_CLASS_TRACKING(RenderPassProxy, boost::serialization::track_never)

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

   size_t szTargets;
   ar >> szTargets ;
   size_t szAttachments;
   ar >> szAttachments ;

   targets.reserve(szTargets);
   for (unsigned int i = 0; i < szTargets; ++i) {
       RenderTargetProxy obj;
       ar >> obj;
       targets.emplace_back(IRenderTarget::Clone(obj));
   }

   targets.reserve(szAttachments);
   for (unsigned int i = 0; i < szAttachments; ++i) {
       RenderTargetProxy obj;
       ar >> obj;
       attachments.emplace_back(IRenderTarget::Clone(obj));
   }

   std::cout << "Serializing RenderPassProxy " << _objId << std::endl;
}

template<class Archive> void RenderPassProxy::save( Archive& ar, const unsigned int version ) const {
   std::cout << "Serializing RenderPassProxy " << _objId << " with " << targets.size() << " targets" << std::endl;
   // We need to serialize a 'proxy' class in order to maintain platform independent
   // scene files. When we desrialize the proxy class, we use the factory method
   // to re-instantiate a platform specific implementation.
   ar << _objId;
   ar << loadOps;

   size_t szTargets {targets.size()};
   ar << szTargets;
   size_t szAttachments{ attachments.size() };
   ar << szAttachments;
   
   // Memory leak but we don't care. This will ensure we get a unique address for each proxy object so
   // boost doesn't try to skip serializing the object. If we create the proxy on the stack, it's the 
   // same address every time (at at least often) so boost thinks it's the same object.
   for (auto& t : targets) {
       ar << *(new RenderTargetProxy(*t));
   }

   for (auto& t : attachments) {
       ar << *(new RenderTargetProxy(*t));
   }
}







