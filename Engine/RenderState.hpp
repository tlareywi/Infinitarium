//
//  PipelineState.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#pragma once

#include <vector>

#include "RenderProgram.hpp"
#include "RenderPass.hpp"

class IRenderCommand;

class IRenderState {
public:
   IRenderState() {}
   virtual ~IRenderState() {
       renderProgram = nullptr;
   }
   
   static std::shared_ptr<IRenderState> Create();
   
   void setProgram( std::shared_ptr<IRenderProgram>& p );
   IRenderProgram& getProgram() {
      return *renderProgram;
   }
   
   void prepare( IRenderContext& context, IRenderCommand& commmand );
   void apply( IRenderPass& );
   virtual void prepareImpl( IRenderContext& context, IRenderCommand& commmand ) = 0;
   virtual void applyImpl( IRenderPass& ) = 0;

private:
   std::shared_ptr<IRenderProgram> renderProgram;
};
