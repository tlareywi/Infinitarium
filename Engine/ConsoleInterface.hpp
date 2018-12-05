//
//  Header.h
//  Infinitarium
//
//  Created by Trystan (Home) on 11/28/18.
//

#pragma once

#include <string>
#include <tuple>

struct Node {
   std::string key;
   int value;
   int value2;
};

#define STRINGIZE_2(A) #A
#define STRINGIZE(A) STRINGIZE_2(A)

#define REFLECT_MEMBER(OBJ, MEMBER) std::make_pair(STRINGIZE(MEMBER), std::ref(OBJ.MEMBER))

template<typename T, typename F, std::size_t ... Is>
void reflect_impl( T& obj, F fun, std::index_sequence<Is...> ) {
   int x[] = { (fun(std::get<Is>(obj).first, std::get<Is>(obj).second), 0)... };
}

template<typename T, typename F> void reflect( T& obj, F fun ) {
   auto tup = reflect_tuple( obj );
   reflect_impl( tup, fun, std::make_index_sequence<std::tuple_size<decltype(tup)>::value>{} );
}

class IConsole {
public:
   IConsole() {
   }
   virtual ~IConsole() {}
   
protected:
   virtual void reflect() = 0;
};
