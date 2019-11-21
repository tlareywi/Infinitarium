///
///
///

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
   s->load( app->getInstallationRoot() + "/share/Infinitarium/sprite.ieb" );
   s->setLocalScenePath( app->getInstallationRoot() + "/share/Infinitarium/" );
   
   PythonInterpreter* pyInterp = dynamic_cast<PythonInterpreter*>(pyInterpInterface.get());
   pyInterp->setScene( s );
   
   Simulation simulation;
   simulation.setScene( s );
   app->run();
}
