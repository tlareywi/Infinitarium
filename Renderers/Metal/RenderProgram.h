//
//  RenderProgram.h
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 12/31/18.
//

#pragma once

#include "../../Engine/RenderProgram.hpp"

#import <Metal/Metal.h>

///
/// brief Metal implementation of RenderProgram
///
class MetalRenderProgram : public IRenderProgram {
public:
   virtual ~MetalRenderProgram();
   
   void prepare( IRenderState& state ) override;
   
   void apply( IRenderState& state ) override;
   
   void compile( const std::string& name, IRenderContext& context ) override;
   
   void injectUniformStruct( const std::vector<std::pair<std::string, UniformType>>& uniforms ) override;
   
private:
   id<MTLFunction> vertex;
   id<MTLFunction> fragment;
   id<MTLLibrary> program;
   std::string uniformBlock;
};
