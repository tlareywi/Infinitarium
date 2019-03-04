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
   virtual ~IRenderPass() {}
   IRenderPass( const IRenderPass& rp ) {
   //    std::copy(rp.targets.begin(), rp.targets.end(), back_inserter(targets));
   }
   
   static std::shared_ptr<IRenderPass> Create();
   
   virtual void prepare( std::shared_ptr<IRenderContext>& ) {};
   
   virtual void begin( std::shared_ptr<IRenderContext>& ) {};
   virtual void end() {};
   
protected:
   std::vector<std::shared_ptr<IRenderTarget>> targets;
   IRenderPass() {};
   
#if defined ENGINE_BUILD
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      std::cout<<"Serializing IRenderPass"<<std::endl;
     // ar & targets;
   }
#endif
};


#if defined ENGINE_BUILD
namespace boost { namespace serialization {
   template<class Archive> inline void save_construct_data( Archive & ar, const IRenderPass* t, const unsigned int file_version ) {
      std::cout<<"IRenderPass: save_construct_data"<<std::endl;
      // save data required to construct instance
   }
   
   template<class Archive> inline void load_construct_data( Archive & ar, IRenderPass* t, const unsigned int file_version ) {
      // retrieve data from archive required to construct new instance
      std::cout<<"IRenderPass: load_construct_data"<<std::endl;
      t = IRenderPass::Create().get();
   }
}}
#endif

