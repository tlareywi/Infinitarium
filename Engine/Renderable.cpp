//
//  Renderable.cpp
//  Infinitarium
//
//  Created by Trystan (Home) on 10/11/18.
//

#include "Renderable.hpp"
#include "PyUtil.hpp"

IRenderable::IRenderable() : dirty(true) {
   pipelineState = IRenderState::Create();
   renderCommand = IRenderCommand::Create();
   uniformData = nullptr;
}

void IRenderable::update( const glm::mat4& mvp ) {
   if( uniformData )
      uniformData->set( &mvp, sizeof(mvp) );
}

void IRenderable::prepare( IRenderContext& context ) {
   pipelineState->prepare( context );
   
   uniformData = IDataBuffer::Create( context );
   renderCommand->add( uniformData );
   
   unsigned int sizeBytes {sizeof(glm::mat4)};
   for( auto& i : uniforms ) {
      sizeBytes += sizeof(i);
   }
   
   // Reserve enough GPU memory for all uniforms.
   uniformData->reserve( sizeof(sizeBytes) );
   
   dirty = false;
}

void IRenderable::render( IRenderPass& renderPass ) {
   if( dirty ) prepare( *(renderPass.renderContext) );

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


