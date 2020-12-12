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
      Event<Args...>* evt = reinterpret_cast<Event<Args...>*>( &event ); // Hmm, dynamic_cast works here on Xcode11 but not VS2019.
      if (evt)
          call_fun(evt->args, std::index_sequence_for<Args...>());
      else
          assert(false);
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



