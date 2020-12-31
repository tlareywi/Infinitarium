//
//  Simulation.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/10/18.
//

#include "Simulation.hpp"
#include "Application.hpp"
#include "Stats.hpp"
#include "ObjectStore.hpp"

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

void Simulation::update() {
   if( scene )
      scene->update();
}

void Simulation::render() {
   if( scene )
      scene->render();
}

void Simulation::simLoop() {
   const unsigned short sampleSize{ 60 };
   auto frameStart = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double, std::milli> duration(std::chrono::high_resolution_clock::now() - frameStart);
   std::vector<std::chrono::duration<double, std::milli>> frameSamples;
   frameSamples.resize(sampleSize);
   unsigned short sampleIndx = { 0 };
   Stats& stats = Stats::Instance();

   while( 1 ) {
      update();
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

   // Wait for all contexts to finish execution before tearing down resources
   scene->waitOnIdle();

   // Dealloc scenegraph
   scene = nullptr;

   // Dealloc static instance tracking caches
   ITexture::clearRegisteredObjs();
   IRenderTarget::clearRegisteredObjs();
   IRenderPass::clearRegisteredObjs();
   ObjectStore::instance().clear();

   // Deleting the context(s) will destroy the graphics device. Do this after cleaning up other graphics resources.
   IRenderContext::clearRegisteredObjs();

   // Signal main thread exit
   IApplication::Create()->stop();
}
