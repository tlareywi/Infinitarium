//
//  Transform.cpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 3/18/19.
//

#include "Transform.hpp"
#include "UniformType.hpp"

//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/string_cast.hpp>
//#include <glm/ext.hpp>

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(Transform)

Transform::Transform() : transform(glm::mat4(1.0)) {
}

void Transform::update( UpdateParams& params ) {
   params.addModel( transform );
   SceneObject::update( params );
}

void Transform::setMatrix( const glm::mat4& mat ) {
   transform = mat;
}

glm::mat4x4 Transform::getMatrix() {
   return transform;
}

void Transform::identity() {
   transform = glm::mat4(1.0);
}

void Transform::scale( float x, float y, float z ) {
   transform = glm::scale( glm::mat4(1.0), glm::vec3(x, y, z) ) * transform;
}

void Transform::translate( float x, float y, float z ) {
   transform = glm::translate( glm::mat4(1.0), glm::vec3(x, y, z) ) * transform;
}

void Transform::rotate( float degrees, float x, float y, float z ) {
   transform = glm::rotate( glm::mat4(1.0), glm::radians(degrees), glm::vec3(x, y, z)) * transform;
}

namespace boost { namespace serialization {
   template<class Archive> inline void serialize(Archive& ar, Transform& t, unsigned int version) {
      std::cout<<"Serializing Transform"<<std::endl;
      boost::serialization::void_cast_register<Transform,SceneObject>();
      ar & boost::serialization::base_object<SceneObject>(t);
      ar & t.transform;
   }
}}

