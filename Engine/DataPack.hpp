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

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unique_ptr.hpp>

template<typename T> class DataPack;

typedef DataPack<float> DataPack_FLOAT32;
typedef DataPack<uint32_t> DataPack_UINT32;
typedef DataPack<uint16_t> DataPack_UINT16;

typedef std::variant<
DataPack_FLOAT32,
DataPack_UINT32,
DataPack_UINT16> DataPackContainer;

namespace boost {
   namespace serialization {
      template<class Archive> void serialize( Archive& ar, DataPackContainer& t, const unsigned int version ) {
         std::cout<<"Serializing DataPackContainer"<<std::endl;
         std::visit( [&ar](auto& e){ ar & e; }, t );
      }
   }
}

///
/// \brief Type independent large dataset abstraction with move semantics.
///
template<typename T> class DataPack {
public:
   DataPack() {
      
   }
   
   DataPack( unsigned int num ) : container(nullptr) {
      data.reserve( num );
   }
   DataPack( const DataPack<T>& obj ) : data(obj.data), container(nullptr) {
      
   }
   DataPack( DataPack<T>&& obj ) : data(std::move(obj.data)) {
      
   }
   
   DataPack<T>& operator=( DataPack<T>&& obj ) {
      data = std::move(obj.data);
      return *this;
   }
   
   unsigned long sizeBytes() const {
      return data.size() * sizeof(T);
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
   
   void addVec( T r, T g, T b, T a ) {
      data.push_back(r);
      data.push_back(g);
      data.push_back(b);
      data.push_back(a);
   }
   
private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      ar & data;
      std::cout<<"Serializing Databuffer, size "<<data.size()<<std::endl;
   }
   
   std::vector<T> data;
};



