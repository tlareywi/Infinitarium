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
#include <variant>

#include <iostream>

class IE_EXPORT IApplication {
public:
   virtual ~IApplication() {}
   
   static std::shared_ptr<IApplication> Create();
   
   std::string getInstallationRoot() {
      return(INSTALL_ROOT);
   }
   
   std::shared_ptr<IPythonInterpreter> getPythonInterpreter() {
      return pyInterp;
   }
   
   void setPythonInterpreter( std::shared_ptr<IPythonInterpreter>& interp );
   
   void subscribe( const std::string& msg, std::shared_ptr<IDelegate>& delegate );
   void unsubscribe( const std::string& msg );
   void invoke( const std::string& json ) {
      JSONEvent jsonEvent;
      rapidjson::Reader reader;
      rapidjson::StringStream ss(json.c_str());
      reader.Parse(ss, jsonEvent);
      
      // Call on every subscriber of message
      for( auto& i : subscribers ) {
         if( i.first == jsonEvent.name ) {
            i.second->operator()(jsonEvent);
         }
      }
   }
   void invoke( IEvent& event ) {
      for( auto& i : subscribers ) {
         if( i.first == event.getName() )
            i.second->operator()(event);
      }
   }
   
   virtual void run() = 0;
   virtual void stop() = 0;
   virtual void addManipulator( const std::string& id, float, float, float ) = 0;
   virtual void* platformInstance() = 0;
   
private:
   std::shared_ptr<IPythonInterpreter> pyInterp;
   std::vector<std::pair<std::string, std::shared_ptr<IDelegate>>> subscribers;
};
