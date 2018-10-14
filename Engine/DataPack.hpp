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

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

template<typename T> class DataPack;

typedef DataPack<float> DataPack_FLOAT32;
typedef DataPack<uint32_t> DataPack_UINT32;
typedef DataPack<uint16_t> DataPack_UINT16;

typedef std::variant<
DataPack_FLOAT32,
DataPack_UINT32,
DataPack_UINT16> DataPackContainer;

template<typename T> class DataPack {
public:
   DataPack( unsigned int num ) : numElements(num) {
      data = std::make_unique<T[]>(numElements);
   }
   DataPack( const DataPack<T>& obj ) {
      if( data ) {
         data = std::make_unique<T[]>(obj.numElements);
      }
      numElements = obj.numElements;
      memcpy(&data, &(obj.data), sizeBytes());
   }
   DataPack( DataPack<T>&& obj ) {
      data = std::move(obj.data);
      numElements = obj.numElements;
   }
   
   unsigned int sizeBytes() const {
      return numElements * sizeof(T);
   }
   
   void set( const T ptr[] ) {
      memcpy(&data, ptr, sizeBytes());
   }
   
   T* release() {
      return data.release();
      data = nullptr;
      numElements = 0;
   }
   
   operator DataPackContainer() const { return DataPackContainer{this}; } // TODO: this invoke copy?
   
private:
   DataPack() : data(nullptr) {}
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version)
   {
      ar & data;
      ar & numElements;
   }
   
   unsigned int numElements;
   std::unique_ptr<T[]> data;
};



