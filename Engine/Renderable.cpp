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

IRenderable::IRenderable() : programName{"default"} {
   pipelineState = IRenderState::Create();
   renderCommand = IRenderCommand::Create();
   uniformData = nullptr;
   texture = nullptr;
   
   // Set built-in uniforms.
   allUniforms.reserve(30);
   allUniforms.push_back( std::make_pair("modelViewProjectionMatrix", glm::fmat4x4()) );
   allUniforms.push_back( std::make_pair("modelViewMatrix", glm::fmat4x4()) );
   allUniforms.push_back( std::make_pair("viewport", glm::uvec2()) );
}

void IRenderable::prepare( IRenderContext& context ) {
   if( !dirty ) return;
   
   uniformData = IDataBuffer::Create( context );
   uniformData->setUsage( IDataBuffer::Usage::Uniform );
   renderCommand->add( uniformData );
   if( texture != nullptr ) {
      renderCommand->add( texture );
      texture->prepare( context );
   }
   
   glm::uvec2 viewport(context.width(), context.height());
   
   // Merge built-ins with user uniforms
   allUniforms.clear();
   allUniforms.push_back( std::make_pair("modelViewProjectionMatrix", glm::fmat4x4()) );
   allUniforms.push_back( std::make_pair("modelViewMatrix", glm::fmat4x4()) );
   allUniforms.push_back( std::make_pair("viewport", viewport) );
   allUniforms.insert( allUniforms.end(), uniforms.begin(), uniforms.end() );
   
   unsigned int sizeBytes {0};
   for( auto& i : allUniforms ) {
      std::visit( [&sizeBytes](auto& e) {
         sizeBytes += sizeof(e);
      }, i.second );
   }
   
   // Reserve enough GPU memory for all uniforms.
   // TODO: +12 is hack; Double check alignment and padding (+1, see https://vulkan-tutorial.com/Uniform_buffers/Descriptor_pool_and_sets). This trips a validation flag on Metal.
   uniformData->reserve( sizeBytes + 12 );
   uniformData->set( &viewport, sizeof(glm::fmat4x4) * 2, sizeof(viewport) );
  
   std::shared_ptr<IRenderProgram> shader = IRenderProgram::Create();
   shader->injectUniformStruct( allUniforms );
   shader->compile( programName, context );
   pipelineState->setProgram( shader );
   
   pipelineState->prepare( context, *renderCommand );
   
   dirty = false;
}

void IRenderable::update(UpdateParams& params) {
    if (!uniformData) return;

    uint32_t offset{ 0 };

    // Built-ins
    glm::fmat4x4 mvp = params.getMVP();
    uniformData->set(&mvp, offset, sizeof(mvp));
    offset += sizeof(mvp);
    glm::fmat4x4 mv = params.getView() * params.getModel();
    uniformData->set(&mv, offset, sizeof(mv));
    offset += sizeof(mv);
    offset += sizeof(glm::uvec2); // viewport

    // Update values for all uniforms
    for (auto& i : uniforms) {
        std::visit([&offset, this](auto& e) {
            uint32_t sz{ sizeof(e) };
            uniformData->set(&e, offset, sz);
            offset += sz;
        }, i.second);
    }

    uniformData->commit(); // Copy to GPU
}

void IRenderable::render( IRenderPass& renderPass ) {
   pipelineState->apply( renderPass );
   renderCommand->encode( renderPass, *pipelineState );
}

void IRenderable::setTexture( const std::shared_ptr<ITexture>& t ) {
   texture = t;
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
         std::shared_ptr<IDelegate> delegate = std::make_shared<JSONDelegate<decltype(fun)>>( fun );
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

template<class Archive> void IRenderable::save( Archive& ar ) const {
   ar << boost::serialization::make_nvp("SceneObject", boost::serialization::base_object<SceneObject>(*this));

   ar << BOOST_SERIALIZATION_NVP(uniforms);
   ar << BOOST_SERIALIZATION_NVP(programName);

   bool hasTextureResource{ false };
  
   if( texture == nullptr )
	   ar << BOOST_SERIALIZATION_NVP( hasTextureResource );
   else {
      hasTextureResource = true;
      ar << BOOST_SERIALIZATION_NVP( hasTextureResource );
      std::unique_ptr<TextureProxy> t = std::make_unique<TextureProxy>(*texture);
      ar << BOOST_SERIALIZATION_NVP(t);
   }
}

template<class Archive> void IRenderable::load( Archive& ar ) {
   ar >> boost::serialization::make_nvp("SceneObject", boost::serialization::base_object<SceneObject>(*this));
   ar >> BOOST_SERIALIZATION_NVP(uniforms);
   ar >> BOOST_SERIALIZATION_NVP(programName); 
   
   bool hasTextureResource{ false };
   ar >> BOOST_SERIALIZATION_NVP(hasTextureResource);
   if( hasTextureResource ) {
      std::unique_ptr<TextureProxy> t;
      ar >> BOOST_SERIALIZATION_NVP(t);
      texture = ITexture::Clone( *t ); 
   }
}

namespace boost { namespace serialization {
   template<class Archive> inline void load(Archive& ar, IRenderable& t, unsigned int version) {
      std::cout<<"Loading IRenderable"<<std::endl;
      t.load( ar );
   }
   template<class Archive> inline void save(Archive& ar, const IRenderable& t, unsigned int version) {
      std::cout<<"Saving IRenderable"<<std::endl;
      t.save( ar );
   }
}}

template<class Archive> void IRenderable::serialize(Archive& ar, unsigned int version) {
	boost::serialization::void_cast_register<IRenderable, SceneObject>();
	boost::serialization::split_free(ar, *this, version);
}

template<class Archive> void ClearScreen::serialize(Archive& ar, unsigned int version) {
	boost::serialization::void_cast_register<ClearScreen, IRenderable>();
	ar & boost::serialization::make_nvp("IRenderable", boost::serialization::base_object<IRenderable>(*this));
}


