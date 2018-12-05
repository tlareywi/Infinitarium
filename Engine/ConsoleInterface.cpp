//
//  ConsoleInterface.cpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 11/28/18.
//

#include "ConsoleInterface.hpp"

auto reflect_tuple( Node& n ) {
   return make_tuple( REFLECT_MEMBER(n, key), REFLECT_MEMBER(n, value), REFLECT_MEMBER(n, value2) );
}


