//
//  PipelineState.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#include "RenderState.hpp"
#include "Module.hpp"

std::shared_ptr<IRenderState> IRenderState::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createRenderState();
}

void IRenderState::setProgram( std::shared_ptr<IRenderProgram>& p ) {
   renderProgram = p;
}

void IRenderState::prepare( IRenderContext& context, IRenderCommand& command ) {
   if( renderProgram )
      renderProgram->prepare( *this );
   
   prepareImpl( context, command );
}

void IRenderState::apply( IRenderPass& renderPass ) {
   if( renderProgram )
      renderProgram->apply( *this );
   
   applyImpl( renderPass );
}
