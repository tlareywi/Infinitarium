//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
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
