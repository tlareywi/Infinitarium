//
//  RenderPass.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#pragma once

#include <memory>
#include <vector>
#include <iostream>

#include "RenderContext.hpp"
#include "Texture.hpp"

#include "../config.h"

class IRenderPass {
public:
   virtual ~IRenderPass() {
   }
   IRenderPass( const IRenderPass& rp ) : _objId(rp._objId) {
      std::copy(rp.targets.begin(), rp.targets.end(), std::back_inserter(targets));
   }
   
   static std::shared_ptr<IRenderPass> Create();
   static std::shared_ptr<IRenderPass> Clone( const IRenderPass& );
   
   void addRenderTarget( const std::shared_ptr<IRenderTarget>& t ) {
      targets.push_back( t );
   }
   
   virtual void prepare( IRenderContext& ) = 0;
   virtual void begin( IRenderContext& ) = 0;
   virtual void end( IRenderContext& ) = 0;
   
   void getData( unsigned int indx, const glm::uvec4& rect, void* data ) {
      targets[indx]->getData( rect, data );
   }
   
   void postRenderOperation( std::function<void(IRenderPass&)>& op ) {
      postRenderOps.push_back( op );
   }
   
   const std::vector<std::shared_ptr<IRenderTarget>>& getRenderTargets() const {
      return targets;
   }
   
   void runPostRenderOperations() {
      for( auto& f : postRenderOps )
         f( *this );
      
      postRenderOps.clear();
   }

protected:
   IRenderPass() : _objId(reinterpret_cast<unsigned long long>(this)) {};
   std::vector<std::shared_ptr<IRenderTarget>> targets;
   unsigned long long _objId;
   
private:
   std::vector<std::function<void(IRenderPass&)>> postRenderOps;
};

///
/// \brief Proxy for serialize/deserialize
///
class RenderPassProxy : public IRenderPass {
public:
   RenderPassProxy() {}
   RenderPassProxy( const IRenderPass& obj ) : IRenderPass(obj) {}
   
   void prepare( IRenderContext& ) override {};
   void begin( IRenderContext& ) override {};
   void end( IRenderContext& ) override {};
   
#if defined ENGINE_BUILD
   friend class boost::serialization::access;
   template<class Archive> void load( Archive &, const unsigned int );
   template<class Archive> void save(Archive&, const unsigned int) const;
   BOOST_SERIALIZATION_SPLIT_MEMBER()
#endif
};

#if defined ENGINE_BUILD
BOOST_CLASS_EXPORT_KEY(RenderPassProxy);
#endif





