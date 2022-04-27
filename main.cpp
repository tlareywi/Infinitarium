//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "config.h"

#include "Engine/Scene.hpp"
#include "Engine/Application.hpp"
#include "Engine/Simulation.hpp"
#include "Engine/PythonBridge.hpp"

int main(int argc, const char * argv[]) {
   std::shared_ptr<IPythonInterpreter> pyInterpInterface = std::make_shared<PythonInterpreter>();
   std::shared_ptr<IApplication> app = IApplication::Create();
   app->setPythonInterpreter( pyInterpInterface );
   
   std::shared_ptr<Scene> s = std::make_shared<Scene>();
   try {
	   s->load(app->getInstallationRoot() + "/share/Infinitarium/imgui.ieb");
   }
   catch( std::exception& e ) {
	   std::cout << e.what() << std::endl;
	   return -1;
   }
   s->setLocalScenePath( app->getInstallationRoot() + "/share/Infinitarium/" );
   
   PythonInterpreter* pyInterp = dynamic_cast<PythonInterpreter*>(pyInterpInterface.get());
   pyInterp->setScene( s );
   
   std::shared_ptr<Simulation> simulation = std::make_shared<Simulation>();
   simulation->setScene( s );
  
   // Don't hang on to references here. Prevents clean exit.
   Scene* scene_ptr = s.get();
   pyInterpInterface = nullptr;
   s = nullptr;
   
   app->run( *scene_ptr );

   simulation->wait();
   app->destroy();
   app = nullptr;
}
