//
//  ISimulation.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/8/18.
//

#pragma once

#include "../config.h"

#include "ApplicationWindow.hpp"
#include "EngineInterface.hpp"
#include "Delegate.hpp"

#include <memory>
#include <string>
#include <vector>

class IApplication {
public:
   virtual ~IApplication() {}
   
   static std::shared_ptr<IApplication> Create();
   
   std::string getInstallationRoot() {
      return(INSTALL_ROOT);
   }
   
   std::shared_ptr<IPythonInterpreter> getPythonInterpreter() {
      return pyInterp;
   }
   
   void setPythonInterpreter( std::shared_ptr<IPythonInterpreter>& interp ) {
      pyInterp = interp;
   }
   
   void subscribe( const std::string& msg, std::shared_ptr<IDelegate>& delegate ) {
      subscribers.push_back(std::make_pair(msg, delegate));
   }
   void unsubscribe( const std::string& msg ) {
      for( auto itr = subscribers.begin(); itr != subscribers.end(); ++itr ) {
         if( itr->first == msg ) {
            itr = subscribers.erase( itr );
            --itr;
         }
      }
   }
   void invoke( const std::string& json ) {
      // TODO: Decode json to list of arguments
      std::string msg("manipulate");
      
      // Call on every subscriber of message
      for( auto& i : subscribers ) {
         if( i.first == msg )
            i.second->operator()();
      }
   }
   
   virtual void run() = 0;
   virtual void stop() = 0;
   
private:
   std::shared_ptr<IPythonInterpreter> pyInterp;
   std::vector<std::pair<std::string, std::shared_ptr<IDelegate>>> subscribers;
};
