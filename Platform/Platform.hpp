//
//  Simulator.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#pragma once

#include <thread>
#include "../Engine/Scene.hpp"

namespace Infinitarium {
   
   class IPlatform {
   public:
      static IPlatform* Create();
      
      IPlatform();
      virtual ~IPlatform() {
         if( simulation != nullptr && simulation->joinable() )
            simulation->join();
      }
      
      void terminate() {
         std::unique_lock<std::mutex> lock(simMutex);
         exiting = true;
      }
      
      void setScene( std::shared_ptr<Scene>& s ) {
         scene = s;
      }
      
      virtual void run() = 0;
      
   protected:
      void update() {
         if( scene )
            scene->update();
      }
      virtual void simLoop();
      virtual void render() = 0;
      
      std::shared_ptr<Scene> scene;
      
   private:
      std::unique_ptr<std::thread> simulation;
      std::mutex simMutex;
      bool exiting;
   };
   
   class PlatformNoImpl : public IPlatform {
   public:
      void run() override {}
      
   protected:
      void render() override {}
   };
   
   class PlatformOSX : public IPlatform {
   public:
      PlatformOSX();
      void run() override;
      
   protected:
      void render() override;
   };
   
}
