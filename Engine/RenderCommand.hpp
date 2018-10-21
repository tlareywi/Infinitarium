//
//  RenderCommand.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#pragma once

#include "DataBuffer.hpp"
#include "RenderPass.hpp"

class IRenderCommand {
public:
   static std::shared_ptr<IRenderCommand> Create();
   
   void add( std::shared_ptr<IDataBuffer>& );
   
   virtual void encode( IRenderPass& ) = 0;
   
protected:
   std::vector<std::shared_ptr<IDataBuffer>> dataBuffers;
};
