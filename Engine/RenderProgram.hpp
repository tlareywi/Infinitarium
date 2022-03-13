//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "RenderContext.hpp"
#include "UniformType.hpp"

class IRenderState;

class IRenderProgram {
public:
   IRenderProgram() {}
   virtual ~IRenderProgram() {}
   
   static std::shared_ptr<IRenderProgram> Create();
   
   virtual void prepare( IRenderState& ) = 0;
   virtual void apply( IRenderState& ) = 0;
   virtual void compile( const std::string& path, IRenderContext& ) = 0;
   virtual void injectUniformStruct( const std::vector<std::pair<std::string, Uniform>>& ) = 0;
};

class IVertexProgram : public IRenderProgram {
public:
private:
};

class IFragmentProgram : public IRenderProgram {
public:
private:
};
