//
//  Transform.cpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 3/18/19.
//

#include "Transform.hpp"
#include "UniformType.hpp"

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(Transform)

void Transform::update( const glm::mat4& mat ) {
   SceneObject::update( transform * mat );
}

void Transform::setMatrix( const glm::mat4& mat ) {
   transform = mat;
}

glm::mat4x4 Transform::getMatrix() {
   return transform;
}

namespace boost { namespace serialization {
   template<class Archive> inline void serialize(Archive& ar, Transform& t, unsigned int version) {
      std::cout<<"Serializing Transform"<<std::endl;
      boost::serialization::void_cast_register<Transform,SceneObject>();
      boost::serialization::base_object<SceneObject>(t);
      ar & t.transform;
   }
}}

