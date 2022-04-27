//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "Simulation.hpp"
#include "Application.hpp"
#include "Stats.hpp"
#include "Database.hpp"

Simulation::Simulation() {
   simulation = std::make_unique<std::thread>( &Simulation::simLoop, this );
}

Simulation::~Simulation() {
   if( simulation != nullptr && simulation->joinable() )
      simulation->join();
}

void Simulation::setScene( std::shared_ptr<Scene>& s ) {
   scene = s;
}

void Simulation::update(const ReferenceTime& rt) {
   if( scene )
      scene->update(rt);
}

void Simulation::render() {
   if( scene )
      scene->render();
}

void Simulation::wait() {
    while (simLoopRunning) {
        std::this_thread::yield();
    }
}

void Simulation::simLoop() {
   simLoopRunning = true;

   const unsigned short sampleSize{ 60 };
   auto frameStart = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double, std::milli> duration(std::chrono::high_resolution_clock::now() - frameStart);
   std::vector<std::chrono::duration<double, std::milli>> frameSamples;
   frameSamples.resize(sampleSize);
   unsigned short sampleIndx = { 0 };
   Stats& stats = Stats::Instance();

   auto appStart = std::chrono::high_resolution_clock::now();

   while( 1 ) {
      ReferenceTime referenceTime(std::chrono::high_resolution_clock::now() - appStart);

      update(referenceTime);
      render();
      
      if(scene && scene->isTerminatePending())
        break;

      // Measure frame duration just prior to yield
      frameSamples[sampleIndx] = std::chrono::high_resolution_clock::now() - frameStart;
     
      double sum{ 0 };
      for (auto& sample : frameSamples)
          sum += sample.count();
      sum /= sampleSize;
      sampleIndx = ++sampleIndx % sampleSize;
      stats.fps = trunc(1000.0 / sum);

      std::this_thread::yield();

      // Start fps clock after we have regained execution
      frameStart = std::chrono::high_resolution_clock::now();
   }

   // Signal main thread exit
   IApplication::Create()->stop();

   // Wait for all contexts to finish execution before tearing down resources
   scene->waitOnIdle();

   // Dealloc scenegraph
   scene = nullptr;

   Database::destroy();

   simLoopRunning = false;
}
