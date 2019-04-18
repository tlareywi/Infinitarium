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
#include <iostream>

#include <fstream>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/unique_ptr.hpp>

#if defined ENGINE_BUILD
   #include <boost/python.hpp>
   #include "Python.h"
#endif

template<typename T> class DataPack;

typedef DataPack<float> DataPack_FLOAT32;
typedef DataPack<uint32_t> DataPack_UINT32;
typedef DataPack<uint16_t> DataPack_UINT16;
typedef DataPack<uint8_t> DataPack_UINT8;

typedef std::variant<
DataPack_FLOAT32,
DataPack_UINT32,
DataPack_UINT16,
DataPack_UINT8> DataPackContainer;

///
/// \brief Type independent large dataset abstraction with move semantics.
///
template<typename T> class DataPack {
public:
   DataPack() {
      
   }
   
   ~DataPack() {
      data.clear();
   }
   
   DataPack( unsigned int num ) : container(nullptr) {
      data.reserve( num );
   }
   DataPack( const DataPack<T>& obj ) : container(nullptr), data(obj.data) {
      
   }
   DataPack( DataPack<T>&& obj ) : data(std::move(obj.data)) {
      
   }
   
   DataPack<T>& operator=( DataPack<T>&& obj ) {
      data = std::move(obj.data);
      return *this;
   }
   
   T operator[]( uint32_t indx ) {
      return data[indx];
   }
   
   unsigned long sizeBytes() const {
      return data.size() * sizeof(T);
   }
   
   unsigned long capacityBytes() const {
      return data.capacity() * sizeof(T);
   }
   
   unsigned int size() const {
      return data.size();
   }
   
   T* get() {
      if( data.size() < 1 )
         return nullptr;
      else
         return &(data[0]);
   }
   
   operator DataPackContainer() {
      return DataPackContainer{ std::move(*this) };
   }
   
   std::unique_ptr<DataPackContainer> container;
   operator DataPackContainer&() {
      if( !container )
         container = std::make_unique<DataPackContainer>(std::move(*this));
      
      return *container;
   }
   
   // Convinience data methods
   void addVec( T x, T y, T z ) {
      data.push_back(x);
      data.push_back(y);
      data.push_back(z);
   }
   
   void add( T v ) {
      data.push_back(v);
   }
   
   void addVec( T r, T g, T b, T a ) {
      data.push_back(r);
      data.push_back(g);
      data.push_back(b);
      data.push_back(a);
   }
   
#if defined ENGINE_BUILD
   boost::python::handle<> getBuffer() {
      data.resize(capacityBytes() / sizeof(T));
      return boost::python::handle<>(PyMemoryView_FromMemory(reinterpret_cast<char*>(data.data()), sizeBytes(), PyBUF_WRITE));
   }
#endif
   
private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      ar & data;
      std::cout<<"Serializing Databuffer, size "<<data.size()<<std::endl;
   }
   
   std::vector<T> data;
};

namespace boost {
   namespace serialization {
      template<class Archive> void serialize( Archive& ar, DataPackContainer& t, const unsigned int version ) {
         std::string type;
         
        try {
            std::get<DataPack_FLOAT32>(t);
            type = "float";
         }
         catch( ... ) {}
         try {
            std::get<DataPack_UINT32>(t);
            type = "uint32";
         }
         catch( ... ) {}
         
         try {
            std::get<DataPack_UINT16>(t);
            type = "uint16";
         }
         catch( ... ) {}
         try {
            std::get<DataPack_UINT8>(t);
            type = "uint8";
         }
         catch( ... ) {}
         
         ar & type;
         
         if( type == "float" ) {
            std::cout<<"Serializing DataPackContainer as float"<<std::endl;
            try {
               ar & std::get<DataPack_FLOAT32>(t);
            }
            catch(...) {
               t = DataPack_FLOAT32();
               ar & std::get<DataPack_FLOAT32>(t);
            }
         }
         else if( type == "uint32" ) {
            std::cout<<"Serializing DataPackContainer as uint32"<<std::endl;
            try {
               ar & std::get<DataPack_UINT32>(t);
            }
            catch(...) {
               t = DataPack_UINT32();
               ar & std::get<DataPack_UINT32>(t);
            }
         }
         else if( type == "uint16" ) {
            std::cout<<"Serializing DataPackContainer as uint16"<<std::endl;
            try {
               ar & std::get<DataPack_UINT16>(t);
            }
            catch(...) {
               t = DataPack_UINT16();
               ar & std::get<DataPack_UINT16>(t);
            }
         }
         else if( type == "uint8" ) {
            std::cout<<"Serializing DataPackContainer as uint8"<<std::endl;
            try {
               ar & std::get<DataPack_UINT8>(t);
            }
            catch(...) {
               t = DataPack_UINT8();
               ar & std::get<DataPack_UINT8>(t);
            }
         }
      }
   }
}




