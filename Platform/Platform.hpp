//
//  Simulator.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#pragma once

#include <thread>

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
      
      virtual void run() = 0;
      
   protected:
      virtual void simLoop();
      virtual void drawFrame() = 0;
      
   private:
      std::unique_ptr<std::thread> simulation;
      std::mutex simMutex;
      bool exiting;
   };
   
   class PlatformNoImpl : public IPlatform {
   public:
      void run() override {}
      
   protected:
      void drawFrame() override {}
   };
   
   class PlatformOSX : public IPlatform {
   public:
      PlatformOSX();
      void run() override;
      
   protected:
      void drawFrame() override;
   };
   
}
