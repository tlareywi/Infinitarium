//
//  ISimulation.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/8/18.
//

#include "Application.hpp"
#include "Module.hpp"

std::shared_ptr<IApplication> IApplication::Create() {
   return ModuleFactory<PlatformFactory>::Instance()->createApplication();
}
