//
//  main.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#include "Platform.hpp"

#include "../Engine/PointCloud.hpp"
#include "../Engine/Scene.hpp"

using namespace Infinitarium;

int main(int argc, const char * argv[]) {
   IPlatform* impl = IPlatform::Create();
   
   std::shared_ptr<Scene> s = std::make_shared<Scene>();
   s->load("/Users/trystan/tyco2.ieb");
   std::shared_ptr<IMotionController> ctrl{ std::make_shared<Orbit>() };
   s->setMotionController( ctrl );
   impl->setScene( s );
   
   impl->run();
   impl->terminate();
   delete impl;
   impl = nullptr;
}


