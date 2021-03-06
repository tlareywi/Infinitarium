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
#include "Scene.hpp"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(IRenderable)
BOOST_CLASS_EXPORT_IMPLEMENT(ClearScreen)

IRenderable::IRenderable() : programName{"default"} {
   UniformType t; // Need an explicit instance on gcc for some reason before we make more references to this type. Lazy template eval ???

   pipelineState = IRenderState::Create();
   renderCommand = IRenderCommand::Create();
   uniformData = nullptr;
   texture = nullptr;
   
   // Set built-in uniforms.
   allUniforms.reserve(30);

   // TODO: need these declared here?
   allUniforms.push_back( std::make_pair("modelViewProjectionMatrix", Uniform(UniformType(glm::fmat4x4()))));
   allUniforms.push_back( std::make_pair("modelViewMatrix", Uniform(UniformType(glm::fmat4x4()))));
   allUniforms.push_back( std::make_pair("viewport", Uniform(UniformType(glm::uvec2()))));
}

void IRenderable::prepare( IRenderContext& context ) {
   // Time delta for shader level animations/effects
   setUniform("referenceTime", Uniform(UniformType(0.0f), UniformType(0.0f), UniformType(10000000000000.f)));

   uniformData = IDataBuffer::Create( context );
   uniformData->setUsage( IDataBuffer::Usage::Uniform );
   renderCommand->add( uniformData );

   std::shared_ptr<IDataBuffer> pick{ context.pickBuffer() };
   renderCommand->add( pick );
   std::shared_ptr<IDataBuffer> postProc{ context.postProcBuffer() };
   renderCommand->add( postProc );

   if( texture != nullptr ) {
      renderCommand->add( texture );
      texture->prepare( context );
   }
   
   viewport = glm::uvec2(context.width(), context.height());
   
   // Merge built-ins with user uniforms
   allUniforms.clear();
   allUniforms.push_back( std::make_pair("modelViewProjectionMatrix", Uniform(UniformType(glm::fmat4x4()))) );
   allUniforms.push_back( std::make_pair("modelViewMatrix", Uniform(UniformType(glm::fmat4x4()))) );
   allUniforms.push_back( std::make_pair("viewport", Uniform(UniformType(viewport))) );
   allUniforms.insert( allUniforms.end(), uniforms.begin(), uniforms.end() );
   
   unsigned int sizeBytes {0};
   for( auto& i : allUniforms ) {
      std::visit( [&sizeBytes](auto& e) {
         sizeBytes += sizeof(e);
      }, i.second.val );
   }
   
   // Reserve enough GPU memory for all uniforms.
   // TODO: +12 is hack; Double check alignment and padding (see https://vulkan-tutorial.com/Uniform_buffers/Descriptor_pool_and_sets). This trips a validation flag on Metal.
   uniformData->reserve( sizeBytes + 12 );
   uniformData->set( &viewport, sizeof(glm::fmat4x4) * 2, sizeof(viewport) );
  
   std::shared_ptr<IRenderProgram> shader = IRenderProgram::Create();
   shader->injectUniformStruct( allUniforms );
   shader->compile( programName, context );
   pipelineState->setProgram( shader );

   pipelineState->setCullMode( cullMode );
   
   pipelineState->prepare( context, *renderCommand );
}

void IRenderable::update(UpdateParams& params) {
    if (!uniformData) return;

    uint32_t offset{ 0 };

    UniformType seconds{ (float)(params.getScene().referenceTime().count() / 1000.0) };
    setUniform("referenceTime", Uniform(UniformType(seconds), UniformType(0.0f), UniformType(10000000000000.f)));

    // Built-ins
    glm::fmat4x4 mvp = params.getMVP();
    uniformData->set(&mvp, offset, sizeof(mvp));
    offset += sizeof(mvp);
    glm::fmat4x4 mv = params.getView() * params.getModel();
    uniformData->set(&mv, offset, sizeof(mv));
    offset += sizeof(mv);
    offset += sizeof(glm::uvec2); // viewport

    // Update values for all uniforms
    // TODO: avoid loop values not dirty
    for (auto& i : uniforms) {
        std::visit([&offset, this](auto& e) {
            uint32_t sz{ sizeof(e) };
            uniformData->set(&e, offset, sz);
            offset += sz;
        }, i.second.val);
    }
}

void IRenderable::render( IRenderPass& renderPass ) {
   uniformData->commit(); // Copy to GPU
   pipelineState->apply( renderPass );
   renderCommand->encode( renderPass, *pipelineState );
}

void IRenderable::setTexture( const std::shared_ptr<ITexture>& t ) {
   texture = t;
}

void IRenderable::setUniform( const std::string& name, const Uniform& value ) {
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

void IRenderable::listUniforms() {
   PyConsoleRedirect console;
   std::stringstream ss;
   
   for( auto& i : uniforms ) {
      ss << i.first << " ->";
      
      std::visit( [&ss](auto& e) {
         ss << " " << type_name<decltype(e)>();
      }, i.second.val );
      
      ss << std::endl;
   }
   
   console.write( ss.str() );
}

std::vector<std::pair<std::string, Uniform>>& IRenderable::getUniforms() {
    return uniforms;
}

template<class Archive> void IRenderable::save( Archive& ar ) const {
   ar << boost::serialization::make_nvp("SceneObject", boost::serialization::base_object<SceneObject>(*this));

   ar << BOOST_SERIALIZATION_NVP(uniforms);
   ar << BOOST_SERIALIZATION_NVP(programName);
   ar << BOOST_SERIALIZATION_NVP(cullMode);

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
   ar >> BOOST_SERIALIZATION_NVP(cullMode);
   
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


