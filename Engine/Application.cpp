//
//  ISimulation.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/8/18.
//

#include "Application.hpp"
#include "Module.hpp"

std::shared_ptr<IApplication> IApplication::Create() {
   return ModuleFactory<PlatformFactory>::Instance()->createApplication();
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

