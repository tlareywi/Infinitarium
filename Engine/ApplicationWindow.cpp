//
//  SimulationWindow.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/8/18.
//

#include "ApplicationWindow.hpp"
#include "Module.hpp"

std::shared_ptr<IApplicationWindow> IApplicationWindow::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createApplicationWindow();
}



