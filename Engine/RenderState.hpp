//
//  PipelineState.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#pragma once

#include <vector>

#include "RenderProgram.hpp"

class IRenderState {
public:
   IRenderState() : dirty(true) {}
   static std::shared_ptr<IRenderState> Create();
   
   void setProgram( std::shared_ptr<IRenderProgram>& p );
   
   void prepare( IRenderContext& context );
   
   virtual void commit( IRenderContext& ) = 0;
   
   void apply();
private:
   std::shared_ptr<IRenderProgram> renderProgram;
   bool dirty;
};
