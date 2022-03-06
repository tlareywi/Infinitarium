//
//  Renderable.hpp
//  Infinitarium
//
//  Created by Trystan (Home) on 10/11/18.
//

#pragma once

#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>
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
#include "Texture.hpp"

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
      texture = nullptr;
   }
   virtual void render( IRenderPass& );
   virtual void prepare( IRenderContext& );
   virtual void update( UpdateParams& );
   
   void setProgram( const std::string& name ) {
      programName = name;
   }

   void setCullMode(IRenderState::CullMode mode) {
       cullMode = mode;
   }
   
   void setTexture( const std::shared_ptr<ITexture>& );

   void addSampler( const std::shared_ptr<IRenderTarget>& );
   
   void setUniform( const std::string&, const Uniform& );
   void listUniforms();
   void removeUniform( const std::string& );
   std::vector<std::pair<std::string, Uniform>>& getUniforms();
   
   auto reflect() {  // IConsole /////////////////////
      IRenderable& obj = *this;
      static auto tup = make_tuple(
         REFLECT_METHOD(&IRenderable::setProgram, setProgram),
         REFLECT_METHOD(&IRenderable::setUniform, setUniform),
         REFLECT_METHOD(&IRenderable::listUniforms, listUniforms),
         REFLECT_METHOD(&IRenderable::removeUniform, removeUniform),
         REFLECT_MEMBER(obj, programName)
      );
      
      return tup;
   }
   
   template<class Archive> void save( Archive& ) const;
   template<class Archive> void load( Archive& );
   
protected:
   std::shared_ptr<IRenderState> pipelineState;
   std::shared_ptr<IRenderCommand> renderCommand;
   glm::uvec2 viewport;
   
private:
   std::vector<std::pair<std::string, Uniform>> allUniforms; // built-ins, not serialized
   std::vector<std::pair<std::string, Uniform>> uniforms;
   std::shared_ptr<IDataBuffer> uniformData;
   std::shared_ptr<ITexture> texture;  
   std::vector<std::shared_ptr<IRenderTarget>> samplers;
   
   // TODO: At some point there needs to be a pre-processing step at startup where
   // renderables with compatible state share the same RenderState instance, and thus share render pipelines. 
   std::string programName;
   IRenderState::CullMode cullMode{IRenderState::CullMode::None};
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive&, const unsigned int);
};

class ClearScreen : public IRenderable {
public:
	ClearScreen() {}

private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive&, const unsigned int);
};

BOOST_CLASS_EXPORT_KEY(IRenderable);
BOOST_CLASS_EXPORT_KEY(ClearScreen);
