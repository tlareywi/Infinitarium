//
//  UniformType.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 12/9/18.
//

#pragma once

#include <variant>
#include <iostream>

#include <boost/archive/xml_woarchive.hpp>
#include <boost/archive/xml_wiarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/binary_object.hpp>

#include <glm/glm.hpp>

typedef std::variant<
float, int, unsigned int,
glm::ivec2, glm::ivec3, glm::ivec4,
glm::mat4x4,
glm::uvec2, glm::uvec3, glm::uvec4,
glm::vec2, glm::vec3, glm::vec4
> UniformType;

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
         std::visit( [&ar](auto& e){ ar & BOOST_SERIALIZATION_NVP(e); }, t );
      }
      
      template<class Archive, class T> void save( Archive& ar, const T& t, const unsigned int version ) {
         ar & boost::serialization::make_nvp("Uniform", boost::serialization::make_binary_object( (void*)&t, sizeof(t) ));
      }
      template<class Archive, class T> void load( Archive& ar, T& t, const unsigned int version ) {
         ar & boost::serialization::make_nvp("Uniform", boost::serialization::make_binary_object( (void*)&t, sizeof(t) ));
      }
   }
}

