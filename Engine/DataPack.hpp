//
//  DataPack.h
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#pragma once

#include <memory>
#include <string>
#include <variant>

template<typename T, int S> class DataPack {
public:
   enum PixelFormat {
      RGBAF32,
      RGBF32,
      RGBU8,
      RGBAU8
   };
   
   DataPack( unsigned int numPoints ) {
      switch( pixelFormat ) {
         case RGBAF32:
            data = std::make_unique<T>(numPoints * 4);
            break;
         case RGBF32:
            data = std::make_unique<T>(numPoints * 3);
            break;
         case RGBU8:
            data = std::make_unique<T>(numPoints * 3);
            break;
         case RGBAU8:
         default:
            data = std::make_unique<T>(numPoints * 4);
            break;
      }
   }
   DataPack( const DataPack& ) { };
   DataPack( const DataPack&& ) { };
   
   void setSampler( const std::string& name ) {
      sampler = name;
   }
   
private:
   std::string sampler;
   DataPack() {}
   PixelFormat pixelFormat;
   std::unique_ptr<T> data;
};

typedef DataPack<uint8_t, DataPack<void, 0>::PixelFormat::RGBAU8> DataPack_RGBAU8;
typedef DataPack<float, DataPack<void, 0>::PixelFormat::RGBAF32> DataPack_RGBAF32;
typedef DataPack<uint8_t, DataPack<void, 0>::PixelFormat::RGBU8> DataPack_RGBU8;
typedef DataPack<float, DataPack<void, 0>::PixelFormat::RGBF32> DataPack_RGBF32;

typedef std::variant<DataPack_RGBAU8,
   DataPack_RGBAF32,
   DataPack_RGBU8,
   DataPack_RGBF32> DataPackContainer;

