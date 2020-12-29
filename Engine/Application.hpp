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

#include "imgui.h"

#include <memory>
#include <string>
#include <vector>
#include <variant>

#include <iostream>

class IApplication {
public:
   virtual ~IApplication() {
       ImGui::DestroyContext();
       subscribers.clear();
       pyInterp = nullptr;
   }
   
   IE_EXPORT static std::shared_ptr<IApplication> Create();
   
   std::string getInstallationRoot() {
      return(INSTALL_ROOT);
   }
   
   std::shared_ptr<IPythonInterpreter> getPythonInterpreter() {
      return pyInterp;
   }
   
   IE_EXPORT void setPythonInterpreter( std::shared_ptr<IPythonInterpreter>& interp );
   
   void subscribe( const std::string& msg, std::shared_ptr<IDelegate>& delegate );
   void unsubscribe( const std::string& msg );

   void invoke( IEvent& event ) {
      std::size_t val = std::hash<std::string>{}(event.getName());

      for( auto& i : subscribers ) {
         if( i.first == val )
            i.second->operator()(event);
      }
   }
   
   virtual void run() = 0;
   virtual void stop() = 0;
   virtual void addManipulator( const std::string& id, float, float, float ) = 0;
   virtual void* platformInstance() = 0;

protected:
    IApplication() {
        // Just always init this with the app. Otherwise we get asserts when checkng state for events when ImGui is not included in the scene. 
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
    }
   
private:
   std::shared_ptr<IPythonInterpreter> pyInterp;
   std::vector<std::pair<std::size_t, std::shared_ptr<IDelegate>>> subscribers;
};
