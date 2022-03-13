//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "Application.hpp"
#include "Module.hpp"
#include "Scene.hpp"

std::shared_ptr<IApplication> IApplication::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createApplication();
}

void IApplication::run( Scene& scene ) {
    while (running) {
        if (!terminatePending)
            scene.mainThread();

        platformRun();
        std::this_thread::yield();
    }
}

void IApplication::setPythonInterpreter( std::shared_ptr<IPythonInterpreter>& interp ) {
   pyInterp = interp;
}

void IApplication::subscribe( const std::string& msg, std::shared_ptr<IDelegate>& delegate ) {
   subscribers.push_back(std::make_pair(std::hash<std::string>{}(msg), delegate));
}

void IApplication::unsubscribe( const std::string& msg ) {
   std::size_t val = std::hash<std::string>{}(msg);

   for( auto itr = subscribers.begin(); itr != subscribers.end(); ++itr ) {
      if( itr->first == val ) {
         itr = subscribers.erase( itr );
         --itr;
      }
   }
}

