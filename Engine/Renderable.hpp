//
//  Renderable.hpp
//  Infinitarium
//
//  Created by Trystan (Home) on 10/11/18.
//

#pragma once

#include <boost/archive/polymorphic_xml_woarchive.hpp>
#include <boost/archive/polymorphic_xml_wiarchive.hpp>
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
#include "SceneObject.hpp"

#include <glm/glm.hpp>

///
/// \brief Base class for anything visible rendered by the underlying 3D graphics API
///
class IRenderable : public SceneObject, public Reflection::IConsole<IRenderable> {
public:
   IRenderable();
   virtual ~IRenderable() {
      pipelineState = nullptr;
      renderCommand = nullptr;
      uniformData = nullptr;
   }
   virtual void render( IRenderPass& );
   virtual void prepare( IRenderContext& );
   virtual void update( const glm::mat4& );
   
   void setProgram( const std::string& name ) {
      programName = name;
   }
   
   void setDirty() {
      dirty = true;
   }
   
   void setUniform( const std::string&, UniformType );
   void listUniforms();
   void removeUniform( const std::string& );
   void manipulateUniform( const std::string&, float, float, float );
   
   auto reflect() {  // IConsole /////////////////////
      IRenderable& obj = *this;
      static auto tup = make_tuple(
         REFLECT_METHOD(&IRenderable::setProgram, setProgram),
         REFLECT_METHOD(&IRenderable::setUniform, setUniform),
         REFLECT_METHOD(&IRenderable::listUniforms, listUniforms),
         REFLECT_METHOD(&IRenderable::removeUniform, removeUniform),
         REFLECT_METHOD(&IRenderable::manipulateUniform, manipulateUniform),
         REFLECT_METHOD(&IRenderable::setDirty, setDirty),
         REFLECT_MEMBER(obj, programName)
      );
      
      return tup;
   }
   
protected:
   std::shared_ptr<IRenderState> pipelineState;
   std::shared_ptr<IRenderCommand> renderCommand;
   bool dirty;
   
private:
   std::vector<std::pair<std::string, UniformType>> allUniforms; // built-ins, not serialized
   std::vector<std::pair<std::string, UniformType>> uniforms;
   std::shared_ptr<IDataBuffer> uniformData;
   std::string programName;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      std::cout<<"Serializing IRenderable"<<std::endl;
      boost::serialization::void_cast_register<IRenderable,SceneObject>();
      ar & BOOST_SERIALIZATION_NVP(uniforms);
      ar & BOOST_SERIALIZATION_NVP(programName);
   }
};

class IE_EXPORT ClearScreen : public IRenderable {
public:
   
private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      std::cout<<"Serializing ClearScreen"<<std::endl;
      boost::serialization::void_cast_register<ClearScreen,IRenderable>();
      ar & boost::serialization::make_nvp("IRenderable", boost::serialization::base_object<IRenderable>(*this));
   }
};
