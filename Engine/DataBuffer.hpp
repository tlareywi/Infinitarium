//
//  Texture.hpp
//  Infinitaruim-Engine
//
//  Created by Trystan (Home) on 10/12/18.
//

#pragma once

#include "DataPack.hpp"

#include <memory>

class IDataBuffer;

class IDataBuffer {
public:
   static std::shared_ptr<IDataBuffer> Create();
   
   virtual void set( DataPackContainer& ) = 0;
};
