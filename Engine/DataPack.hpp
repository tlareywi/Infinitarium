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

#if defined ENGINE_BUILD
   #include <boost/python.hpp>
   #include "Python.h"
#endif

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
   
   DataPack( unsigned int num ) {
      data.reserve( num );
   }
   DataPack( const DataPack<T>& obj ) : data(obj.data) {
      
   }
   DataPack( DataPack<T>&& obj ) : data(std::move(obj.data)) {
      
   }
   
   DataPack<T>& operator=( DataPack<T>&& obj ) {
      data = std::move(obj.data);
      return *this;
   }
   
   size_t sizeBytes() const {
      return data.size() * sizeof(T);
   }
   
   size_t capacityBytes() const {
      return data.capacity() * sizeof(T);
   }
   
   T* get() {
      if( data.size() < 1 )
         return nullptr;
      else
         return &(data[0]);
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
      return boost::python::handle<>(PyMemoryView_FromMemory(reinterpret_cast<char *>(data.data()), capacityBytes(), PyBUF_WRITE));
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

typedef DataPack<float> DataPack_FLOAT32;
typedef DataPack<uint32_t> DataPack_UINT32;
typedef DataPack<uint16_t> DataPack_UINT16;
typedef DataPack<uint8_t> DataPack_UINT8;

typedef std::variant< DataPack_FLOAT32,
	DataPack_UINT32,
	DataPack_UINT16,
	DataPack_UINT8
> DataPackContainer;

template<typename T> DataPackContainer wrapDataPack( DataPack<T>& dataPack ) {
	return { std::move(dataPack) };
}

namespace boost {
	namespace serialization {
		template<class Archive> void serialize(Archive& ar, DataPackContainer& t, const unsigned int version) {
			std::cout << "Serializing DataPackContainer" << std::endl;
			std::visit([&ar](auto & e) { ar& e; }, t);
		}
	}
}

