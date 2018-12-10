//
//  UniformType.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 12/9/18.
//

#pragma once

typedef std::variant<
float, double, int, unsigned int,
glm::dmat4x4,
glm::dvec2, glm::dvec3, glm::dvec4,
glm::ivec2, glm::ivec3, glm::ivec4,
glm::mat4x4,
glm::uvec2, glm::uvec3, glm::uvec4,
glm::vec2, glm::vec3, glm::vec4
> UniformType;

BOOST_SERIALIZATION_SPLIT_FREE(glm::dmat4x4)
BOOST_SERIALIZATION_SPLIT_FREE(glm::dvec2)
BOOST_SERIALIZATION_SPLIT_FREE(glm::dvec3)
BOOST_SERIALIZATION_SPLIT_FREE(glm::dvec4)
BOOST_SERIALIZATION_SPLIT_FREE(glm::uvec2)
BOOST_SERIALIZATION_SPLIT_FREE(glm::uvec3)
BOOST_SERIALIZATION_SPLIT_FREE(glm::uvec4)
BOOST_SERIALIZATION_SPLIT_FREE(glm::ivec2)
BOOST_SERIALIZATION_SPLIT_FREE(glm::ivec3)
BOOST_SERIALIZATION_SPLIT_FREE(glm::ivec4)
BOOST_SERIALIZATION_SPLIT_FREE(glm::vec2)
BOOST_SERIALIZATION_SPLIT_FREE(glm::vec3)
BOOST_SERIALIZATION_SPLIT_FREE(glm::vec4)
BOOST_SERIALIZATION_SPLIT_FREE(glm::mat4x4)

namespace boost {
   namespace serialization {
      template<class Archive> void serialize( Archive& ar, UniformType& t, const unsigned int version ) {
         std::cout<<"Serializing UniformType"<<std::endl;
         std::visit( [&ar](auto& e){ ar & e; }, t );
      }
      
      template<class Archive, class T> void save( Archive& ar, const T& t, const unsigned int version ) {
         char buf[sizeof(t)];
         memcpy( &buf, &t, sizeof(t) );
         ar & buf;
      }
      template<class Archive, class T> void load( Archive& ar, T& t, const unsigned int version ) {
         char buf[sizeof(t)];
         ar & buf;
         memcpy( &t, &buf, sizeof(t) );
      }
   }
}
