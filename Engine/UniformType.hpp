//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include <variant>
#include <iostream>

#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/binary_object.hpp>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include "ConsoleInterface.hpp"

using UniformType = std::variant<
float, int, unsigned int,
glm::ivec2, glm::ivec3, glm::ivec4,
glm::mat4x4,
glm::uvec2, glm::uvec3, glm::uvec4,
glm::vec2, glm::vec3, glm::vec4
>;

BOOST_SERIALIZATION_SPLIT_FREE(glm::uvec2)
BOOST_SERIALIZATION_SPLIT_FREE(glm::uvec3)
BOOST_SERIALIZATION_SPLIT_FREE(glm::uvec4)
BOOST_SERIALIZATION_SPLIT_FREE(glm::ivec2)
BOOST_SERIALIZATION_SPLIT_FREE(glm::ivec3)
BOOST_SERIALIZATION_SPLIT_FREE(glm::ivec4)
BOOST_SERIALIZATION_SPLIT_FREE(glm::vec2)
BOOST_SERIALIZATION_SPLIT_FREE(glm::vec3)
BOOST_SERIALIZATION_SPLIT_FREE(glm::dvec3)
BOOST_SERIALIZATION_SPLIT_FREE(glm::vec4)
BOOST_SERIALIZATION_SPLIT_FREE(glm::mat4x4)

namespace boost {
   namespace serialization {
      template<class Archive> void serialize( Archive& ar, UniformType& t, const unsigned int version ) {
         std::cout<<"Serializing UniformType"<<std::endl;
         std::visit( [&ar](auto& e){ ar & BOOST_SERIALIZATION_NVP(e); }, t );
      }
      
      template<class Archive, class T> void save( Archive& ar, const T& t, const unsigned int version ) {
         ar & boost::serialization::make_nvp("Uniform Value", boost::serialization::make_binary_object( (void*)&t, sizeof(t) ));
      }
      template<class Archive, class T> void load( Archive& ar, T& t, const unsigned int version ) {
         ar & boost::serialization::make_nvp("Uniform Value", boost::serialization::make_binary_object( (void*)&t, sizeof(t) ));
      }
   }
}

struct Uniform {
    Uniform() {}

    Uniform(const UniformType& v) :
        val(v),
        min(0u),
        max(0u) {
    }
    Uniform(const UniformType& v, const UniformType& mn, const UniformType& mx) :
        val(v), 
        min(mn), 
        max(mx) {
    }
    UniformType val;
    UniformType min;
    UniformType max;
    size_t offset;
};

namespace boost {
    namespace serialization {
        template<class Archive> void serialize(Archive& ar, Uniform& t, const unsigned int version) {
            std::cout << "Serializing Uniform" << std::endl;
            ar & BOOST_SERIALIZATION_NVP(t.val);
            ar & BOOST_SERIALIZATION_NVP(t.min);
            ar & BOOST_SERIALIZATION_NVP(t.max);
        }
    }
}


