//
//  ISimulation.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/8/18.
//

#pragma once

#include "../config.h"

#include "ApplicationWindow.hpp"
#include "EngineInterface.hpp"
#include "Delegate.hpp"

#include <memory>
#include <string>
#include <vector>

#include "rapidjson/reader.h"
#include <iostream>

using namespace rapidjson;

struct MyHandler {
   bool Null() { std::cout << "Null()" << std::endl; return true; }
   bool Bool(bool b) { std::cout << "Bool(" << std::boolalpha << b << ")" << std::endl; return true; }
   bool Int(int i) { std::cout << "Int(" << i << ")" << std::endl; return true; }
   bool Uint(unsigned u) { std::cout << "Uint(" << u << ")" << std::endl; return true; }
   bool Int64(int64_t i) { std::cout << "Int64(" << i << ")" << std::endl; return true; }
   bool Uint64(uint64_t u) { std::cout << "Uint64(" << u << ")" << std::endl; return true; }
   bool Double(double d) { std::cout << "Double(" << d << ")" << std::endl; return true; }
   bool RawNumber(const char* str, SizeType length, bool copy) {
      std::cout << "Number(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
      return true;
   }
   bool String(const char* str, SizeType length, bool copy) {
      std::cout << "String(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
      return true;
   }
   bool StartObject() { std::cout << "StartObject()" << std::endl; return true; }
   bool Key(const char* str, SizeType length, bool copy) {
      std::cout << "Key(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
      return true;
   }
   bool EndObject(SizeType memberCount) { std::cout << "EndObject(" << memberCount << ")" << std::endl; return true; }
   bool StartArray() { std::cout << "StartArray()" << std::endl; return true; }
   bool EndArray(SizeType elementCount) { std::cout << "EndArray(" << elementCount << ")" << std::endl; return true; }
};

class IApplication {
public:
   virtual ~IApplication() {}
   
   static std::shared_ptr<IApplication> Create();
   
   std::string getInstallationRoot() {
      return(INSTALL_ROOT);
   }
   
   std::shared_ptr<IPythonInterpreter> getPythonInterpreter() {
      return pyInterp;
   }
   
   void setPythonInterpreter( std::shared_ptr<IPythonInterpreter>& interp ) {
      pyInterp = interp;
   }
   
   void subscribe( const std::string& msg, std::shared_ptr<IDelegate>& delegate ) {
      subscribers.push_back(std::make_pair(msg, delegate));
   }
   void unsubscribe( const std::string& msg ) {
      for( auto itr = subscribers.begin(); itr != subscribers.end(); ++itr ) {
         if( itr->first == msg ) {
            itr = subscribers.erase( itr );
            --itr;
         }
      }
   }
   void invoke( const std::string& json ) {
      // TODO: Decode json to list of arguments
      std::string msg("manipulate");
      
      std::tuple args = std::make_tuple();
      std::tuple_cat( args, std::make_tuple(0.01) );
      Event evt(args);
      
      // Call on every subscriber of message
      for( auto& i : subscribers ) {
         if( i.first == msg ) {
            i.second->operator()(evt);
         }
      }
   }
   
   virtual void run() = 0;
   virtual void stop() = 0;
   
private:
   std::shared_ptr<IPythonInterpreter> pyInterp;
   std::vector<std::pair<std::string, std::shared_ptr<IDelegate>>> subscribers;
};
