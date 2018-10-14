//
//  Texture.cpp
//  Infinitaruim-Engine
//
//  Created by Trystan (Home) on 10/12/18.
//

#include "DataBuffer.hpp"

// TODO Move to runtime loaded lib. Once that's done, remove dependency on AppKit framework.
extern std::shared_ptr<IDataBuffer> CreateMetalDataBuffer();

std::shared_ptr<IDataBuffer> IDataBuffer::Create() {
   return CreateMetalDataBuffer();
}
