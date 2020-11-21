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

//#include <boost/archive/polymorphic_xml_woarchive.hpp>
//#include <boost/archive/polymorphic_xml_wiarchive.hpp>
#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/unique_ptr.hpp>

#if defined ENGINE_BUILD
   #include <boost/python.hpp>
   #include "Python.h"
#endif

#include "../config.h"

enum class DataType {
    FLOAT,
    UINT8,
    UINT16,
    UINT32
};

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
   
   T operator[]( uint32_t indx ) {
      return data[indx];
   }
   
   unsigned long sizeBytes() const {
      return (unsigned long)data.size() * sizeof(T); // TODO: Remove cast and return size_t
   }
   
   unsigned long capacityBytes() const {
      return (unsigned long)data.capacity() * sizeof(T);
   }
   
   unsigned int size() const {
      return (unsigned int)data.size(); // TODO: Remove cast and return size_t
   }
   
   T* get() {
      if( data.size() < 1 )
         return nullptr;
      else
         return &(data[0]);
   }

   uint32_t getStride() {
       return channelsPerVertex * sizeof(T);
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

   void setChannelsPerVertex( uint8_t channels ) {
       channelsPerVertex = channels;
   }

   uint8_t getChannelsPerVertex() {
       return channelsPerVertex;
   }

   DataType getType() {
      if(std::is_same<T, uint8_t>::value)
         return DataType::UINT8;
      else if (std::is_same<T, uint16_t>::value)
         return DataType::UINT16;
      else if (std::is_same<T, uint32_t>::value)
         return DataType::UINT32;

      return DataType::FLOAT;
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
      ar & BOOST_SERIALIZATION_NVP(data);
      ar & BOOST_SERIALIZATION_NVP(channelsPerVertex);
      std::cout<<"Serializing Databuffer, size "<<data.size()<<std::endl;
   }
   
   std::vector<T> data;
   uint8_t channelsPerVertex;
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

template<typename T> DataPackContainer& wrapDataPack( DataPack<T>& dataPack ) {
   std::unique_ptr<DataPackContainer> container{ std::make_unique<DataPackContainer>(std::move(dataPack)) };
   return *(container.release());
}

namespace boost {
   namespace serialization {
      template<class Archive> void serialize( Archive& ar, DataPackContainer& t, const unsigned int version ) {
         std::string type;
         
        try {
			DataPackContainer val = std::get<DataPack_FLOAT32>(t);
            type = "float";
         }
         catch( ... ) {}
         try {
			 DataPackContainer val  = std::get<DataPack_UINT32>(t);
             type = "uint32";
         }
         catch( ... ) {}
         
         try {
			 DataPackContainer val = std::get<DataPack_UINT16>(t);
             type = "uint16";
         }
         catch( ... ) {}
         try {
			 DataPackContainer val = std::get<DataPack_UINT8>(t);
             type = "uint8";
         }
         catch( ... ) {}
         
         ar & BOOST_SERIALIZATION_NVP(type);
         
         if( type == "float" ) {
            std::cout<<"Serializing DataPackContainer as float"<<std::endl;
            try {
               ar & boost::serialization::make_nvp("DataPack_FLOAT32", std::get<DataPack_FLOAT32>(t));
            }
            catch(...) {
               t = DataPack_FLOAT32();
               ar & boost::serialization::make_nvp("DataPack_FLOAT32", std::get<DataPack_FLOAT32>(t));
            }
         }
         else if( type == "uint32" ) {
            std::cout<<"Serializing DataPackContainer as uint32"<<std::endl;
            try {
               ar & boost::serialization::make_nvp("DataPack_UINT32", std::get<DataPack_UINT32>(t));
            }
            catch(...) {
               t = DataPack_UINT32();
               ar & boost::serialization::make_nvp("DataPack_UINT32", std::get<DataPack_UINT32>(t));
            }
         }
         else if( type == "uint16" ) {
            std::cout<<"Serializing DataPackContainer as uint16"<<std::endl;
            try {
               ar & boost::serialization::make_nvp("DataPack_UINT16", std::get<DataPack_UINT16>(t));
            }
            catch(...) {
               t = DataPack_UINT16();
               ar & boost::serialization::make_nvp("DataPack_UINT16", std::get<DataPack_UINT16>(t));
            }
         }
         else if( type == "uint8" ) {
            std::cout<<"Serializing DataPackContainer as uint8"<<std::endl;
            try {
               ar & boost::serialization::make_nvp("DataPack_UINT8", std::get<DataPack_UINT8>(t));
            }
            catch(...) {
               t = DataPack_UINT8();
               ar & boost::serialization::make_nvp("DataPack_UINT8", std::get<DataPack_UINT8>(t));
            }
         }
      }
   }
}




