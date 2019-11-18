//
//  Renderable.cpp
//  Infinitarium
//
//  Created by Trystan (Home) on 10/11/18.
//

#include "Renderable.hpp"
#include "PyUtil.hpp"
#include "Application.hpp"
#include "Delegate.hpp"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(IRenderable)
BOOST_CLASS_EXPORT_IMPLEMENT(ClearScreen)

IRenderable::IRenderable() : dirty(true) {
   pipelineState = IRenderState::Create();
   renderCommand = IRenderCommand::Create();
   uniformData = nullptr;
   
   // Set built-in uniforms.
   allUniforms.reserve(30);
   allUniforms.push_back( std::make_pair("modelViewProjectionMatrix", glm::mat4()) );
}

void IRenderable::update( const glm::mat4& mvp ) {
   if( !uniformData ) return;

   uint32_t offset {0};
   
   // Built-ins
   uniformData->set( &mvp, offset, sizeof(mvp) );
   offset += sizeof(mvp);
   
   // Update values for all uniforms
   for( auto& i : uniforms ) {
      std::visit( [&offset, this](auto& e) {
         uint32_t sz {sizeof(e)};
         uniformData->set( &e, offset, sz );
         offset += sz;
      }, i.second );
   }
   
   uniformData->commit(); // Copy to GPU
}

void IRenderable::prepare( IRenderContext& context ) {
   if( !dirty ) return;
   
   uniformData = IDataBuffer::Create( context );
   renderCommand->add( uniformData );
   
   // Merge built-ins with user uniforms
   allUniforms.clear();
   allUniforms.push_back( std::make_pair("modelViewProjectionMatrix", glm::mat4()) );
   allUniforms.insert( allUniforms.end(), uniforms.begin(), uniforms.end() );
   
   unsigned int sizeBytes {0};
   for( auto& i : allUniforms )
      std::visit( [&sizeBytes](auto& e){ sizeBytes += sizeof(e); }, i.second );
   
   // Reserve enough GPU memory for all uniforms.
   uniformData->reserve( sizeBytes );
  
   if( !programName.empty() ) {
      std::shared_ptr<IRenderProgram> shader = IRenderProgram::Create();
      shader->injectUniformStruct( allUniforms );
      shader->compile( programName, context );
      pipelineState->setProgram( shader );
   }
   
   pipelineState->prepare( context );
   
   dirty = false;
}

void IRenderable::render( IRenderPass& renderPass ) {
   pipelineState->apply();
   renderCommand->encode( renderPass, *pipelineState );
}

void IRenderable::setUniform( const std::string& name, UniformType value ) {
   for( auto& i : uniforms ) {
      if( i.first == name ) {
         i.second = value;
         return;
      }
   }
   
   // Not found, create new
   dirty = true;
   uniforms.push_back( std::make_pair(name, value) );
}

void IRenderable::removeUniform( const std::string& name ) {   
   for( auto itr = uniforms.begin(); itr != uniforms.end(); ++itr ) {
      if( itr->first == name ) {
         uniforms.erase(itr);
         dirty = true;
         break;
      }
   }
}

void IRenderable::manipulateUniform( const std::string& name, float min, float max, float step ) {
   for( auto& i : uniforms ) {
      if( i.first == name ) {
         auto fun = [&i]( JSONEvent::Args& args ) {
            if( args.size() < 1 ) return;
            std::visit(overload{
               [&i](double val) {
                  UniformType v {(float)val};
                  i.second = v;
               },
               [](bool f) {},
               [&i](int val) {
                  UniformType v {val};
                  i.second = v;
               },
               [&i](unsigned int val) {
                  UniformType v {val};
                  i.second = v;
               },
               [](std::string& s) {}
            }, args[0]);
         };
         std::shared_ptr<IDelegate> delegate = std::make_shared<Delegate<decltype(fun), JSONEvent::Args&>>( fun );
         IApplication::Create()->subscribe(name, delegate);
         IApplication::Create()->addManipulator(name, min, max, step);
         break;
      }
   }
}

void IRenderable::listUniforms() {
   PyConsoleRedirect console;
   std::stringstream ss;
   
   for( auto& i : uniforms ) {
      ss << i.first << " ->";
      
      std::visit( [&ss](auto& e) {
         ss << " " << type_name<decltype(e)>();
      }, i.second );
      
      ss << std::endl;
   }
   
   console.write( ss.str() );
}

template<class Archive> void ClearScreen::serialize(Archive& ar, const unsigned int version) {
	std::cout << "Serializing ClearScreen" << std::endl;
	boost::serialization::void_cast_register<ClearScreen, IRenderable>();
	ar& boost::serialization::make_nvp("IRenderable", boost::serialization::base_object<IRenderable>(*this));
}

template<class Archive> void IRenderable::serialize(Archive& ar, const unsigned int version) {
	std::cout << "Serializing IRenderable" << std::endl;
	boost::serialization::void_cast_register<IRenderable, SceneObject>();
	ar & BOOST_SERIALIZATION_NVP(uniforms);
	ar & BOOST_SERIALIZATION_NVP(programName);
}


