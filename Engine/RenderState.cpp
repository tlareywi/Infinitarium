//
//  PipelineState.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#include "PipelineState.hpp"

extern std::shared_ptr<IPipelineState> CreateMetalPipelineState();

std::shared_ptr<IPipelineState> IPipelineState::Create() {
   return CreateMetalPipelineState();
}
