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
   dirty = true;
}

void IRenderState::prepare( IRenderContext& context ) {
   if( !dirty ) return;
   
   if( renderProgram )
      renderProgram->prepare( *this, context );
   
   dirty = false;
   
   commit( context );
}

void IRenderState::apply() {
   if( renderProgram )
      renderProgram->apply( *this );
}
