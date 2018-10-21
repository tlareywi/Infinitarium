//
//  RenderPass.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#pragma once

#include <memory>

class IRenderPass {
public:
   static std::shared_ptr<IRenderPass> Create();
   
   virtual void begin() = 0;
   virtual void end() = 0;
};
