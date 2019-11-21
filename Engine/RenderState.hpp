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

class IRenderState {
public:
   IRenderState() : dirty(true) {}
   virtual ~IRenderState() {}
   
   static std::shared_ptr<IRenderState> Create();
   
   void setProgram( std::shared_ptr<IRenderProgram>& p );
   IRenderProgram& getProgram() {
      return *renderProgram;
   }
   
   void prepare( IRenderContext& context );
   
   virtual void commit( IRenderContext& ) = 0;
   
   virtual void apply( IRenderPass& );

protected:
	bool dirty;

private:
   std::shared_ptr<IRenderProgram> renderProgram;
};
