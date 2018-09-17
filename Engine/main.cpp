//
//  main.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#include "Platform.hpp"

using namespace Infinitarium;

int main(int argc, const char * argv[]) {
   IPlatform* impl = IPlatform::Create();
   impl->run();
   impl->terminate();
   delete impl;
   impl = nullptr;
}


