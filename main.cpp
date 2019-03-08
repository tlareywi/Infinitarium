///
///
///

#include "config.h"

#include "Engine/PointCloud.hpp"
#include "Engine/Scene.hpp"
#include "Engine/ApplicationWindow.hpp"
#include "Engine/Application.hpp"
#include "Engine/Simulation.hpp"
#include "Engine/MotionControllerOrbit.hpp"
#include "Engine/PythonBridge.hpp"

int main(int argc, const char * argv[]) {
   std::shared_ptr<IPythonInterpreter> pyInterpInterface = std::make_shared<PythonInterpreter>();
   std::shared_ptr<IApplication> app = IApplication::Create();
   app->setPythonInterpreter( pyInterpInterface );
   
   std::shared_ptr<Scene> s = std::make_shared<Scene>();
   s->load("/Users/tlareywi/test.ieb");
   
   PythonInterpreter* pyInterp = dynamic_cast<PythonInterpreter*>(pyInterpInterface.get());
   pyInterp->setScene( s );
   
   Simulation simulation;
   simulation.setScene( s );
   app->run();
   
/*
   std::shared_ptr<IRenderContext> context = IRenderContext::Create( 0, 0, 1920, 1080, false );
 std::shared_ptr<IApplicationWindow> window = IApplicationWindow::Create( *context ); */
}
