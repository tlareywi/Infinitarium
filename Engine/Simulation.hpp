//
//  Simulation.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/10/18.
//

#pragma once

#include "Scene.hpp"

#include <thread>

class IE_EXPORT Simulation {
public:
   Simulation();
   ~Simulation();
   
   void setScene( std::shared_ptr<Scene>& s );
   void wait();
   
protected:
   void update(const ReferenceTime&);
   void render();
   
   void simLoop();

   std::shared_ptr<Scene> scene;
   
private:
   std::unique_ptr<std::thread> simulation;
   std::atomic<bool> simLoopRunning{false};
};
