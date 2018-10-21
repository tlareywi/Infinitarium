//
//  PipelineState.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#pragma once

#include <memory>

class IPipelineState {
public:
   static std::shared_ptr<IPipelineState> Create();
private:
};
