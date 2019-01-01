#pragma once

#include "../../Engine/RenderCommand.hpp"

///
/// brief Metal implementation of RenderCommand
///
class MetalRenderCommand : public IRenderCommand {
public:
   void encode( IRenderPass& renderPass, const IRenderState& state ) override;
};
