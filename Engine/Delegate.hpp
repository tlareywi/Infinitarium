//
//  Delegate.h
//  Infinitarium
//
//  Created by Trystan (Home) on 1/18/19.
//

#pragma once

class IEvent
{
public:
   IEvent() {};
   virtual ~IEvent() {};
};

template<typename... Args> class Event : public IEvent {
public:
   Event(std::tuple<Args...>& a) : args(a) {};
   ~Event() = default;

   std::tuple<Args...> args;
};


class IDelegate {
public:
   virtual ~IDelegate() {}
   virtual void operator()( IEvent& ) = 0;
};

template<typename T, typename... Args> class Delegate : public IDelegate {
public:
   Delegate( T f ) : fun(f) {}
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



