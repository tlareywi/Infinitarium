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
   
   std::string getName() {
      return name;
   }
   void setName( const std::string& n ) {
      name = n;
   }
   
private:
   std::string name;
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
   bool Int(int i) {
      args.push_back((double)i); // TODO: This didn't reall work as planned. Temp workaround is to cast everything to a double. Issue with variant types not able to automatically cast.
      return true;
   }
   bool Uint(unsigned u) {
      args.push_back((double)u);
      return true;
   }
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
/// \brief Delegate for when event parameter types are known at compile time. General implementation.
///
template<typename T, typename... Args> class EventDelegate : public IDelegate {
public:
   EventDelegate( T f ) : fun(f) {}
   void operator()( IEvent& event ) override {
      Event<Args...>* evt = dynamic_cast<Event<Args...>*>( &event );
      if( evt )
         call_fun( evt->args, std::index_sequence_for<Args...>() );
   }
   
private:
   template<std::size_t... Is> void call_fun( const std::tuple<Args...>& tuple, std::index_sequence<Is...> ) {
      fun( std::get<Is>(tuple)... );
   }
   
   T fun; // T is an std::function
};

///
/// \brief JSON events have unknown types at compile time. Can't handle this type of event quite as generally.
///
template<typename T> class JSONDelegate : public IDelegate {
public:
   JSONDelegate( T f ) : fun(f) {}
   void operator()( IEvent& event ) override {
      JSONEvent* evt = dynamic_cast<JSONEvent*>( &event ); // Runtime path
      if( evt )
         fun( evt->args );
   }
   
private:
   T fun; // T is an std::function
};

///
/// \brief 'Overload' pattern for visiting variant types.
///
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;



