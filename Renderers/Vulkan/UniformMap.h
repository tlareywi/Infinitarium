//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "../../Engine/UniformType.hpp"

std::ostream& operator<<( std::ostream& os, const UniformType& uniform ) {
   {
      auto val = std::get_if<unsigned int>( &uniform );
      if( val ) {
         os << "uint";
         return os;
      }
   }
   {
      auto val = std::get_if<int>( &uniform );
      if( val ) {
         os << "int";
         return os;
      }
   }
   {
      auto val = std::get_if<float>( &uniform );
      if( val ) {
         os << "float";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::ivec2>( &uniform );
      if( val ) {
         os << "ivec2";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::ivec3>( &uniform );
      if( val ) {
         os << "ivec3";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::ivec4>( &uniform );
      if( val ) {
         os << "ivec4";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::mat4x4>( &uniform );
      if( val ) {
         os << "mat4";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::uvec2>( &uniform );
      if( val ) {
         os << "uvec2";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::uvec3>( &uniform );
      if( val ) {
         os << "uvec3";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::uvec4>( &uniform );
      if( val ) {
         os << "uvec4";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::vec2>( &uniform );
      if( val ) {
         os << "vec2";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::vec3>( &uniform );
      if( val ) {
         os << "vec3";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::vec4>( &uniform );
      if( val ) {
         os << "vec4";
         return os;
      }
   }
   
   return os;
}
