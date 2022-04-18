//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
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
#include <array>
#include <variant>
#include <functional>
#include <mutex>

#include <iostream>

class Scene;

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
   
   IE_EXPORT void run( Scene& scene );
   virtual void platformRun() = 0;

   virtual void runOnUIThread( std::function<void()>& ) = 0;
   virtual void stop() = 0;
   virtual void destroy() = 0;
   virtual void addManipulator( const std::string& id, float, float, float ) = 0;
   virtual void* platformInstance() = 0;

protected:
    IApplication() {
        // Just always init this with the app. Otherwise we get asserts when checkng state for events when ImGui is not included in the scene. 
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
    }

    void pushFunction( std::function<void()>& f ) {
        assert(uiThreadQueueIndx < 30);
        std::scoped_lock lock(threadQLock);
        uiThreadQueue[uiThreadQueueIndx++] = f;
    }
    std::atomic<bool> running{ true };
    std::atomic<bool> terminatePending{ false };
   
private:
   std::array<std::function<void()>, 30> uiThreadQueue;
   size_t uiThreadQueueIndx = 0;
   std::mutex threadQLock;
    
   std::shared_ptr<IPythonInterpreter> pyInterp;
   std::vector<std::pair<std::size_t, std::shared_ptr<IDelegate>>> subscribers;
};
