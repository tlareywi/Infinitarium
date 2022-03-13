//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
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
