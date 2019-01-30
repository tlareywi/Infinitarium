//
//  Delegate.h
//  Infinitarium
//
//  Created by Trystan (Home) on 1/18/19.
//

#pragma once

#include "rapidjson/reader.h"

#include <iostream>
#include <variant>
#include <vector>

///
/// \brief An Event type handled by a Delegate instance.
///
class IEvent
{
public:
   IEvent() {};
   virtual ~IEvent() {};
};

///
/// \brief Encapsulates arguments for an event fired by JS code. Also serves as handler for rapidjson lib parsing.
/// In this type of event, argument number and type are not known until runtime.
///
class JSONEvent : public IEvent {
public:
   typedef std::variant<
   std::string,
   bool,
   int,
   unsigned,
   double> JSONType;
   
   typedef std::vector<JSONType> Args;
      
   JSONEvent() : isId(false) {
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
   bool RawNumber(const char* str, rapidjson::SizeType length, bool copy) { return true; }
      bool String(const char* str, rapidjson::SizeType length, bool copy) {
         if( isId )
            name = str;
         else
            args.push_back(std::string(str));
         
         return true;
      }
   bool StartObject() { return true; }
   bool Key(const char* str, rapidjson::SizeType length, bool copy) {
      if( strcmp(str, "id") == 0 )
         isId = true;
      else
         isId = false;
      
      return true;
   }
   bool EndObject(rapidjson::SizeType memberCount) { return true; }
   bool StartArray() { return true; }
   bool EndArray(rapidjson::SizeType elementCount) { return true; }
   
   bool isId;
   std::string name;
   std::vector<JSONType> args;
};

///
/// \brief An Event with variable arguments known at compile time (fired from C++ rather than JS).
///
template<typename... Args> class Event : public IEvent {
public:
   Event(std::tuple<Args...>& a) : args(a) {};
   ~Event() = default;

   std::tuple<Args...> args;
};

///
/// \brief Simple interface for a Delegate which handles an Event.
///
class IDelegate {
public:
   virtual ~IDelegate() {}
   virtual void operator()( IEvent& ) = 0;
};

///
/// \brief A Delegate that can handle either compile time or runtime variable argument events.
///
template<typename T, typename... Args> class Delegate : public IDelegate {
public:
   Delegate( T f ) : fun(f) {}
   void operator()( IEvent& event ) override {
      Event<Args...>* evt = dynamic_cast<Event<Args...>*>( &event );
      if( evt )
         call_fun( evt->args, std::index_sequence_for<Args...>() ); // Compile time path
      else {
         JSONEvent* evt = dynamic_cast<JSONEvent*>( &event ); // Runtime path
         if( evt )
            fun( evt->args );
      }
   }
   
private:
   template<std::size_t... Is> void call_fun( const std::tuple<Args...>& tuple, std::index_sequence<Is...> ) {
      fun( std::get<Is>(tuple)... );
   }
   
   T fun; // T is an std::function
};

///
/// \brief 'Overload' pattern for visiting variant types.
///
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;



