//
//  RenderPass.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#pragma once

#include <memory>

#include "RenderContext.hpp"
#include "Texture.hpp"

class IRenderPass {
public:
   virtual ~IRenderPass() {}
   
   static std::shared_ptr<IRenderPass> Create();
   
   virtual void begin() = 0;
   virtual void end() = 0;
   
   std::shared_ptr<IRenderContext> renderContext;
   std::vector<RenderTarget> targets;
};
