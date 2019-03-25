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

class IRenderPass {
public:
   virtual ~IRenderPass() {
   }
   IRenderPass( const IRenderPass& rp ) {
      std::copy(rp.targets.begin(), rp.targets.end(), std::back_inserter(targets));
   }
   
   static std::shared_ptr<IRenderPass> Create();
   static std::shared_ptr<IRenderPass> Clone( const IRenderPass& );
   
   void addRenderTarget( const std::shared_ptr<IRenderTarget>& t ) {
      targets.push_back( t );
   }
   
   virtual void prepare( std::shared_ptr<IRenderContext>& ) = 0;
   virtual void begin( std::shared_ptr<IRenderContext>& ) = 0;
   virtual void end() = 0;

protected:
   IRenderPass() {};
   
   std::vector<std::shared_ptr<IRenderTarget>> targets;
};

///
/// \brief Proxy for serialize/deserialize
///
class RenderPassProxy : public IRenderPass {
public:
   RenderPassProxy() {}
   RenderPassProxy( const IRenderPass& obj ) : IRenderPass(obj) {}
   template<class Archive> void save( Archive& ) const;
   template<class Archive> void load( Archive& );
   
   void prepare( std::shared_ptr<IRenderContext>& ) override {};
   void begin( std::shared_ptr<IRenderContext>& ) override {};
   void end() override {};
   
#if defined ENGINE_BUILD
   friend class boost::serialization::access;
   template<class Archive> friend void boost::serialization::serialize( Archive &, RenderPassProxy&, unsigned int );
#endif
};





