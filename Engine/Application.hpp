//
//  ISimulation.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/8/18.
//

#pragma once

#include "ApplicationWindow.hpp"
#include <memory>

class IApplication {
public:
   virtual ~IApplication() {}
   
   static std::shared_ptr<IApplication> Create();
   
   virtual void run() = 0;
   virtual void stop() = 0;
};
