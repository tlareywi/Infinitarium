//
//  Simulator.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#include "Platform.hpp"

#include <iostream>
#include <thread>

#if __APPLE__
#include <dispatch/dispatch.h>
void EntryPointOSXMainThread();
void DrawFrameOSX();
#endif

using namespace Infinitarium;

IPlatform* IPlatform::Create() {
#if __APPLE__
   return new PlatformOSX();
#else
   return new PlatformNoImpl();
#endif
}

IPlatform::IPlatform() : exiting(false) {
   simulation = std::make_unique<std::thread>( &IPlatform::simLoop, this );
}

void IPlatform::simLoop() {
   while( 1 ) {
      drawFrame();
      
      {
         std::unique_lock<std::mutex> lock(simMutex);
         if( exiting )
            break;
      }
   }
}

PlatformOSX::PlatformOSX() {

}

void PlatformOSX::run() {
#if __APPLE__
   EntryPointOSXMainThread();
#endif
}

void PlatformOSX::drawFrame() {
#if __APPLE__
   dispatch_sync( dispatch_get_main_queue(), ^{ scene->draw(); } );
#endif
}



