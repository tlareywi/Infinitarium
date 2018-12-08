//
//  Header.h
//  Infinitarium
//
//  Created by Trystan (Home) on 11/28/18.
//

#pragma once

#include <tuple>
#include <utility>

#define STRINGIZE_2(A) #A
#define STRINGIZE(A) STRINGIZE_2(A)

#define REFLECT_MEMBER(OBJ, MEMBER) std::make_pair(STRINGIZE(MEMBER), std::ref(OBJ.MEMBER))

template<typename T, typename F, std::size_t ... Is>
void reflect_impl( T& obj, F fun, std::index_sequence<Is...> ) {
   int x[] = { (fun(std::get<Is>(obj).first, std::get<Is>(obj).second), 0)... };
}

template<typename T, typename S, typename F> auto reflect( T& obj, S& tup, F fun ) {
   auto t{ tup };
   return [tup, fun] () {
      reflect_impl( tup, fun, std::make_index_sequence<std::tuple_size<decltype(t)>::value>{} );
   };
}

class IConsole {
public:
   IConsole() {
   }
   virtual ~IConsole() {}
   
protected:
   virtual void reflect() = 0;
   virtual void contains() = 0;
};
