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
#include <variant>

#include "rapidjson/reader.h"
#include <iostream>

using namespace rapidjson;

struct JSONHandler {
   typedef std::variant<
   std::string,
   bool,
   int,
   unsigned,
   int64_t,
   uint64_t,
   double> JSONType;
   
   JSONHandler() : isId(false) {
   }
   
   bool Null() { return true; }
   bool Bool(bool b) { return true; }
   bool Int(int i) { return true; }
   bool Uint(unsigned u) { return true; }
   bool Int64(int64_t i) { return true; }
   bool Uint64(uint64_t u) { return true; }
   bool Double(double d) {
      args.push_back(d);
      return true;
      
   }
   bool RawNumber(const char* str, SizeType length, bool copy) { return true; }
   bool String(const char* str, SizeType length, bool copy) {
      if( isId )
         name = str;
      else
         args.push_back(std::string(str));
      
      return true;
   }
   bool StartObject() { return true; }
   bool Key(const char* str, SizeType length, bool copy) {
      if( strcmp(str, "id") == 0 )
         isId = true;
      else
         isId = false;
      
      return true;
   }
   bool EndObject(SizeType memberCount) { return true; }
   bool StartArray() { return true; }
   bool EndArray(SizeType elementCount) { return true; }
   
   std::vector<JSONType> getArgs() {
      return args;
   }
   
   std::string getName() {
      return name;
   }
   
private:
   bool isId;
   std::string name;
   std::vector<JSONType> args;
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
      JSONHandler handler;
      Reader reader;
      StringStream ss(json.c_str());
      reader.Parse(ss, handler);
      
      std::tuple args = std::make_tuple();
      for( auto& item : handler.getArgs() ) {
         std::visit( [&args](auto& e) {
            std::cout << "adding " << e << std::endl;
            args = std::tuple_cat( args, std::make_tuple(e) );
         }, item );
      }
      
      std::string name{ handler.getName() };
      Event evt(args);
      
      // Call on every subscriber of message
      for( auto& i : subscribers ) {
         if( i.first == name ) {
            i.second->operator()(evt);
         }
      }
   }
   
   virtual void run() = 0;
   virtual void stop() = 0;
   virtual void addManipulator( const std::string& id, float, float, float ) = 0;
   
private:
   std::shared_ptr<IPythonInterpreter> pyInterp;
   std::vector<std::pair<std::string, std::shared_ptr<IDelegate>>> subscribers;
};
