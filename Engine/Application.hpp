//
//  ISimulation.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/8/18.
//

#pragma once

#include <memory>

class IApplication {
public:
   static std::shared_ptr<IApplication> Create();
   
   virtual void run() = 0;
   virtual void stop() = 0;
};
