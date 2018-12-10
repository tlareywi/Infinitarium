//
//  Renderable.hpp
//  Infinitarium
//
//  Created by Trystan (Home) on 10/11/18.
//

#pragma once

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/variant.hpp>

#include <iostream>

#include "RenderCommand.hpp"
#include "RenderState.hpp"
#include "RenderPass.hpp"
#include "RenderContext.hpp"
#include "ConsoleInterface.hpp"
#include "UniformType.hpp"

#include <glm/glm.hpp>

class IRenderable : public Reflection::IConsole<IRenderable> {
public:
   IRenderable();
   virtual ~IRenderable() {}
   virtual void render( IRenderPass& ) = 0;
   virtual void prepare( IRenderContext& );
   virtual void update( const glm::mat4& );
   
   void setUniform( const std::string&, UniformType );
   void listUniforms();
   void removeUniform( const std::string& );
   
   auto reflect() {  // IConsole /////////////////////
      static auto tup = make_tuple(
         REFLECT_METHOD(&IRenderable::setUniform, setUniform),
         REFLECT_METHOD(&IRenderable::listUniforms, listUniforms),
         REFLECT_METHOD(&IRenderable::removeUniform, removeUniform)
      );
      
      return tup;
   }
   
protected:
   std::shared_ptr<IRenderState> pipelineState;
   std::shared_ptr<IRenderCommand> renderCommand;
   std::shared_ptr<IDataBuffer> uniformData;
   std::vector<std::pair<std::string, UniformType>> uniforms;
   bool dirty;
   
private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      std::cout<<"Serializing IRenderable"<<std::endl;
      //ar & uniforms;
   }
};
