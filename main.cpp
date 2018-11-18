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

int main(int argc, const char * argv[]) {
   std::shared_ptr<IApplication> app = IApplication::Create();
   std::shared_ptr<IRenderContext> context = IRenderContext::Create( 100, 100, 1920, 1080, false );
   std::shared_ptr<IApplicationWindow> window = IApplicationWindow::Create( *context );
   
   std::shared_ptr<Scene> s = std::make_shared<Scene>();
   s->setRenderContext( context );
   s->load( app->getInstallationRoot() + "/share/Infinitarium/tyco2.ieb" );
   std::shared_ptr<IMotionController> ctrl{ std::make_shared<Orbit>() };
   s->setMotionController( ctrl );
   
   Simulation simulation;
   simulation.setScene( s );
   app->run();
}
