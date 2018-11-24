//
//  Renderable.hpp
//  Infinitarium
//
//  Created by Trystan (Home) on 10/11/18.
//

#pragma once

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <iostream>

#include "RenderCommand.hpp"
#include "RenderState.hpp"
#include "RenderPass.hpp"
#include "RenderContext.hpp"

class IRenderable {
public:
   IRenderable();
   virtual ~IRenderable() {}
   virtual void render( IRenderPass& ) = 0;
   virtual void prepare( IRenderContext& );
   virtual void update( const glm::mat4& );
   virtual void info() const = 0;
   
protected:
   std::shared_ptr<IRenderState> pipelineState;
   std::shared_ptr<IRenderCommand> renderCommand;
   std::shared_ptr<IDataBuffer> uniforms;
   bool dirty;
   
private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      std::cout<<"Serializing IRenderable"<<std::endl;
   }
};
