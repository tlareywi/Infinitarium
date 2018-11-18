//
//  RenderProgram.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/21/18.
//

#pragma once

#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "RenderContext.hpp"

class IRenderState;

class IRenderProgram {
public:
   IRenderProgram() : dirty(true) {}
   virtual ~IRenderProgram() {}
   
   static std::shared_ptr<IRenderProgram> Create();
   
   virtual void prepare( IRenderState&, IRenderContext& ) = 0;
   virtual void apply( IRenderState& ) = 0;
   
protected:
   bool dirty;
};

class IVertexProgram : public IRenderProgram {
public:
private:
};

class IFragmentProgram : public IRenderProgram {
public:
private:
};
