//
//  Simulation.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/10/18.
//

#pragma once

#include "Scene.hpp"

#include <thread>

class Simulation {
public:
   Simulation();
   ~Simulation();
   
   void terminate();
   void setScene( std::shared_ptr<Scene>& s );
   
protected:
   void update() {
      if( scene )
         scene->update();
   }
   void render() {
      if( scene )
         scene->draw();
   }
   
   void simLoop();

   std::shared_ptr<Scene> scene;
   
private:
   std::unique_ptr<std::thread> simulation;
   std::mutex simMutex;
   bool exiting;
};