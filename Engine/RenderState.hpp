//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include <vector>

#include "RenderProgram.hpp"
#include "RenderPass.hpp"

class IRenderCommand;

class IRenderState {
public:
   enum class CullMode {
       None,
       Front,
       Back
   };

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

   virtual void setCullMode( IRenderState::CullMode ) = 0;

private:
   std::shared_ptr<IRenderProgram> renderProgram;
};
