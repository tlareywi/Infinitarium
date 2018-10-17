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
   DataPack( unsigned int num ) : numElements(num), container(nullptr) {
      data = std::make_unique<T[]>(numElements);
   }
   DataPack( const DataPack<T>& obj ) : container(nullptr) {
      if( data ) {
         data = std::make_unique<T[]>(obj.numElements);
      }
      numElements = obj.numElements;
      memcpy(&data, &(obj.data), sizeBytes());
   }
   DataPack( DataPack<T>&& obj ) : container(nullptr) {
      data = std::move(obj.data);
      numElements = obj.numElements;
      obj.numElements = 0;
   }
   
   DataPack<T>& operator=( DataPack<T>&& obj ) {
      if( container ) {
         delete container;
         container = nullptr;
      }
      data = std::move(obj.data);
      numElements = obj.numElements;
      obj.numElements = 0;
      return *this;
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
   
   operator DataPackContainer() {
      return DataPackContainer{ std::move(*this) };
   }
   
   DataPackContainer* container;
   operator DataPackContainer&() {
      if( !container )
         container = new DataPackContainer{std::move(*this)};
      
      return *container;
   }
   
private:
   DataPack() : data(nullptr), numElements(0) {}
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version)
   {
      ar & data;
      ar & numElements;
   }
   
   unsigned int numElements;
   std::unique_ptr<T[]> data;

};



