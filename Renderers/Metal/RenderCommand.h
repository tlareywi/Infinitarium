#pragma once

#include "../../Engine/RenderCommand.hpp"

#import <Metal/Metal.h>

///
/// brief Metal implementation of RenderCommand
///
class MetalRenderCommand : public IRenderCommand {
public:
   void encode( IRenderPass& renderPass, IRenderState& state ) override;
   void setPrimitiveType( PrimitiveType ) override; 

private:
   MTLPrimitiveType primitiveType;
};
