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
#include <memory>
#include <string>

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
   
   virtual void run() = 0;
   virtual void stop() = 0;
   
private:
   std::shared_ptr<IPythonInterpreter> pyInterp;
};
