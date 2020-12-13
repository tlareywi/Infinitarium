//
//  Simulation.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/10/18.
//

#include "Simulation.hpp"
#include "Application.hpp"

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
   while( 1 ) {
      update();
      render();
      
      if(scene && scene->isTerminatePending())
        break;

      std::this_thread::yield();
   }

   // Wait for all contexts to finish execution before tearing down resources
   scene->waitOnIdle();

   // Dealloc scenegraph
   scene = nullptr;

   // Dealloc static instance tracking caches
   ITexture::clearRegisteredObjs();
   IRenderTarget::clearRegisteredObjs();
   IRenderPass::clearRegisteredObjs();
   IRenderContext::clearRegisteredObjs();

   // Signal main thread exit
   IApplication::Create()->stop();
}
