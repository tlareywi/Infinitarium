//
//  main.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#include "Platform.hpp"

#include "../Engine/PointCloud.hpp"

using namespace Infinitarium;

int main(int argc, const char * argv[]) {
   IPlatform* impl = IPlatform::Create();
   
   
   PointCloud pc;
   DataPack_UINT32 dp(1000);
  //// DataPackContainer dpc { std::move(dp) };
   pc.addVertexBuffer( dp, "position" );
   
   impl->run();
   impl->terminate();
   delete impl;
   impl = nullptr;
}


