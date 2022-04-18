//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "Renderable.hpp"
#include "PyUtil.hpp"
#include "Application.hpp"
#include "Delegate.hpp"
#include "Scene.hpp"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(IRenderable)
BOOST_CLASS_EXPORT_IMPLEMENT(ClearScreen)

IRenderable::IRenderable() : programName{ "default" } {
    UniformType t; // Need an explicit instance on gcc for some reason before we make more references to this type. Lazy template eval ???

    pipelineState = IRenderState::Create();
    renderCommand = IRenderCommand::Create();
    uniformData = nullptr;
    texture = nullptr;

    // Set built-in uniforms.
    allUniforms.reserve(30);

    uniformMutex = std::make_shared<std::mutex>();
}

void IRenderable::prepare( IRenderContext& context ) {
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

   for( auto& sampler : samplers )
       renderCommand->add(sampler);
   
   viewport = glm::uvec2(context.width(), context.height());
   
   // Merge built-ins with user uniforms
   allUniforms.clear();
   allUniforms.push_back( std::make_pair("modelViewProjectionMatrix", Uniform(UniformType(glm::fmat4x4()))) );
   allUniforms.push_back( std::make_pair("modelViewMatrix", Uniform(UniformType(glm::fmat4x4()))) ); 
   allUniforms.push_back( std::make_pair("viewport", Uniform(UniformType(viewport))) );
   allUniforms.push_back(std::make_pair("referenceTime", Uniform(UniformType(0.0f), UniformType(0.0f), UniformType(999999999.0f))) );
   allUniforms.insert( allUniforms.end(), uniforms.begin(), uniforms.end() ); 

   auto comparator = [](std::pair<std::string, Uniform>& a, std::pair<std::string, Uniform>& b) {
       size_t aSize{ 0 };
       size_t bSize{ 0 };
       std::visit([&aSize](auto& e) {
           aSize += sizeof(e);
       }, a.second.val);
       std::visit([&bSize](auto& e) {
           bSize += sizeof(e);
       }, b.second.val);

       return (aSize > bSize);
   };
   std::sort(allUniforms.begin(), allUniforms.end(), comparator);
   
   // Reserve enough GPU memory for all uniforms.
   unsigned int sizeBytes {0};
   for( auto& i : allUniforms ) {
      i.second.offset = sizeBytes;

      std::visit( [&sizeBytes, &i](auto& e) {
         sizeBytes += sizeof(e);
      }, i.second.val );
   }
   uniformData->reserve( sizeBytes );
  
   std::shared_ptr<IRenderProgram> shader = IRenderProgram::Create();
   shader->injectUniformStruct( allUniforms );
   shader->compile( programName, context );
   pipelineState->setProgram( shader );

   pipelineState->setCullMode( cullMode );
   pipelineState->setPolygonMode( polygonMode );
   
   pipelineState->prepare( context, *renderCommand );
}

void IRenderable::update(UpdateParams& params) {
    if (!uniformData) return;

    UniformType seconds{ (float)(params.getScene().referenceTime().count() / 1000.0) };
    updateUniform("referenceTime", Uniform(UniformType(seconds)));

    std::scoped_lock lock{ *uniformMutex };

    // Update values for all uniforms
    for (auto& i : allUniforms) {
        std::string name = i.first;
        size_t offset = i.second.offset;
        std::visit([&name, &params, offset, this](auto& e) {
            uint32_t sz{ sizeof(e) };

            // Built-ins are special case
            if (name == "modelViewProjectionMatrix") {
                glm::fmat4x4 mvp = params.getMVP();
                uniformData->set(&mvp, offset, sizeof(mvp));
            }
            else if (name == "modelViewMatrix") {
                glm::fmat4x4 mv = params.getView() * params.getModel();
                uniformData->set(&mv, offset, sizeof(mv));
            }
            else 
                uniformData->set(&e, offset, sz);
        }, i.second.val);
    }

    SceneObject::update(params);
}

void IRenderable::render( IRenderPass& renderPass ) {
    auto& targets = renderPass.getRenderTargets();
    if (targets.size() > 0) {
        updateUniform( "viewport", Uniform(UniformType(targets[0]->getExtent())) );
    }

    uniformData->commit(); // Copy to GPU
    pipelineState->apply( renderPass );
    renderCommand->encode( renderPass, *pipelineState );

    SceneObject::render(renderPass);
}

void IRenderable::addSampler(const std::shared_ptr<IRenderTarget>& samplerSource) {
    samplers.push_back(samplerSource);
}

void IRenderable::setTexture( const std::shared_ptr<ITexture>& t ) {
   texture = t;
}

void IRenderable::setUniform( const std::string& name, const Uniform& value ) {

   for( auto& i : uniforms) {
       if (i.first == name)
           return; // Don't add same uniform again. Updates should use updateUniform.
   }
   
   dirty = true;
   uniforms.push_back( std::make_pair(name, value) );
}

void IRenderable::updateUniform(const std::string& name, const Uniform& value) {
    std::scoped_lock lock{ *uniformMutex };

    for (auto& i : allUniforms) {
        if (i.first == name) {
            i.second.val = value.val;
            return;
        }
    }

    assert(false); // The uniform should already exist. If not, call setUniform.
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
    return allUniforms;
}

template<class Archive> void IRenderable::save( Archive& ar ) const {
   ar << boost::serialization::base_object<SceneObject>(*this);

   ar << uniforms;
   ar << programName;
   ar << cullMode;
   ar << polygonMode;

   bool hasTextureResource{ false };
  
   if( texture == nullptr )
	   ar << hasTextureResource;
   else {
      hasTextureResource = true;
      ar << hasTextureResource;
      std::unique_ptr<TextureProxy> t = std::make_unique<TextureProxy>(*texture);
      ar << t;
   }

   size_t szSamplers{ samplers.size() };
   ar << szSamplers;

   // Memory leak but we don't care. This will ensure we get a unique address for each proxy object so
   // boost doesn't try to skip serializing the object. If we create the proxy on the stack, it's the 
   // same address every time (at at least often) so boost thinks it's the same object.
   for (auto& s : samplers)
       ar << *(new RenderTargetProxy(*s));
}

template<class Archive> void IRenderable::load( Archive& ar ) {
   ar >> boost::serialization::base_object<SceneObject>(*this);

   ar >> uniforms;
   ar >> programName; 
   ar >> cullMode;
   ar >> polygonMode;
   
   bool hasTextureResource{ false };
   ar >> hasTextureResource;
   if( hasTextureResource ) {
      std::unique_ptr<TextureProxy> t;
      ar >> t;
      texture = ITexture::Clone( *t ); 
   }

   size_t szSamplers;
   ar >> szSamplers;

   samplers.reserve(szSamplers);
   for (unsigned int i = 0; i < szSamplers; ++i) {
       RenderTargetProxy proxy;
       ar >> proxy;
       samplers.emplace_back(IRenderTarget::Clone(proxy));
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
	ar & boost::serialization::base_object<IRenderable>(*this);
}


