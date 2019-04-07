//
//  UniformMap.h
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 12/31/18.
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
         os << "int2";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::ivec3>( &uniform );
      if( val ) {
         os << "int3";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::ivec4>( &uniform );
      if( val ) {
         os << "int4";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::mat4x4>( &uniform );
      if( val ) {
         os << "float4x4";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::uvec2>( &uniform );
      if( val ) {
         os << "uint2";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::uvec3>( &uniform );
      if( val ) {
         os << "uint3";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::uvec4>( &uniform );
      if( val ) {
         os << "uint4";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::vec2>( &uniform );
      if( val ) {
         os << "packed_float2";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::vec3>( &uniform );
      if( val ) {
         os << "packed_float3";
         return os;
      }
   }
   {
      auto val = std::get_if<glm::vec4>( &uniform );
      if( val ) {
         os << "packed_float4";
         return os;
      }
   }
   
   return os;
}
