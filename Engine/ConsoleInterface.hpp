//
//  Header.h
//  Infinitarium
//
//  Created by Trystan (Home) on 11/28/18.
//

#pragma once

#include <tuple>
#include <utility>
#include <string>
#include <type_traits>
#include <typeinfo>

#ifndef _MSC_VER
   #include <cxxabi.h>
#endif

#include <sstream>

#include "PyUtil.hpp"

// Typename demangler (runtime); https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c
template<typename T> std::string type_name() {
   typedef typename std::remove_reference<T>::type TR;
   
   std::unique_ptr<char, void(*)(void*)> own (
#ifndef _MSC_VER
                                              abi::__cxa_demangle(typeid(TR).name(), nullptr,nullptr, nullptr),
#else
                                              nullptr,
#endif
                                              std::free );
   
   std::string r = own != nullptr ? own.get() : typeid(TR).name();
   if (std::is_const<TR>::value)
      r += " const";
   if (std::is_volatile<TR>::value)
      r += " volatile";
   if (std::is_lvalue_reference<T>::value)
      r += "&";
   else if (std::is_rvalue_reference<T>::value)
      r += "&&";
   return r;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace Reflection {

#define STRINGIZE_2(A) #A
#define STRINGIZE(A) STRINGIZE_2(A)

#define REFLECT_MEMBER(OBJ, MEMBER) std::make_pair(STRINGIZE(MEMBER), std::ref(OBJ.MEMBER))
#define REFLECT_METHOD(MEMBER, NAME) std::make_pair(STRINGIZE(NAME), MEMBER)
   
template<typename T> void ignore( const T& ) {}
   
template<typename T, typename F, std::size_t ... Is>
void reflect_impl( const T& tup, F fun, std::index_sequence<Is...> ) {
   int x[] = { (fun(std::get<Is>(tup).first, std::get<Is>(tup).second), 0)... };
   ignore(x);
}

template<typename S, typename F> void apply( const S& tup, F fun ) {
   auto t{ tup };
   reflect_impl( tup, fun, std::make_index_sequence<std::tuple_size<decltype(t)>::value>{} );
}

template<typename S, typename T> void getProp( const S& tup, T& prop, const std::string& propName ) {
   auto fun = [&prop, propName] (const char* name, auto& property) {
      if( propName == name )
         prop = &property;
   };
   
   auto t{ tup };
   reflect_impl( tup, fun, std::make_index_sequence<std::tuple_size<decltype(t)>::value>{} );
}

template<typename T> class IConsole {
public:
   IConsole() {
      child = dynamic_cast<T*>(this);
   }
   virtual ~IConsole() {}
   
   void propList() {
      PyConsoleRedirect console;
      std::stringstream ss;
      
      auto fun = [&ss] (const char* name, auto& property) {
         ss << name << " -> " << type_name<decltype(property)>() << std::endl;
      };
      
      Reflection::apply(child->reflect(), fun);
      console.write( ss.str() );
   }
   
private:
   T* child;
};
   
}
