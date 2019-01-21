//
//  Delegate.h
//  Infinitarium
//
//  Created by Trystan (Home) on 1/18/19.
//

#pragma once

class IDelegate {
public:
   virtual ~IDelegate() {}
   virtual void operator()() = 0;
};

template<typename T, typename... Args> class Delegate : public IDelegate {
public:
   Delegate( T f ) : fun(f) {}
   void operator()() override {
      fun( args... );
   }
   
private:
   std::tuple<Args...> args;
   T fun; // T is an std::function
};


