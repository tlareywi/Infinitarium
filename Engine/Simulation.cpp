//
//  Simulation.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/10/18.
//

#include "Simulation.hpp"

Simulation::Simulation() : exiting(false) {
   simulation = std::make_unique<std::thread>( &Simulation::simLoop, this );
}

Simulation::~Simulation() {
   if( simulation != nullptr && simulation->joinable() )
      simulation->join();
}

void Simulation::terminate() {
   std::unique_lock<std::mutex> lock(simMutex);
   exiting = true;
}

void Simulation::setScene( std::shared_ptr<Scene>& s ) {
   scene = s;
}

void Simulation::simLoop() {
   clock_t time{ 0 };
   const double clocks_per_ms { CLOCKS_PER_SEC / 1000.0 };
   
   while( 1 ) {
      time += clock();
      if( time / clocks_per_ms < 16.6666666667 ) {
         sleep(0);
         continue;
      }
      
      time = 0;
      update();
      render();
      
      {
         std::unique_lock<std::mutex> lock(simMutex);
         if( exiting )
            break;
      }
   }
}
