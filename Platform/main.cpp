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
   
   
   Scene s;
   s.load("/Users/trystan/tyco2.ieb");
   
   impl->run();
   impl->terminate();
   delete impl;
   impl = nullptr;
}


