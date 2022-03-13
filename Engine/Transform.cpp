//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "Transform.hpp"
#include "UniformType.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Transform)

Transform::Transform() : transform(glm::mat4(1.0)) {
}

void Transform::update( UpdateParams& params ) {
   params.addModel( transform );
   SceneObject::update( params );
}

void Transform::setMatrix( const glm::mat4& mat ) {
   transform = mat;
}

void Transform::setTransform( System coords ) {
   glm::dmat4 identity(1.0);
   
   switch (coords) {
      case System::VSOP87:
         transform =  glm::rotate( identity, glm::radians(180.0), glm::dvec3(0, 0, 1) ) *
         glm::rotate( identity, glm::radians(90.0), glm::dvec3(1, 0, 0) );
         break;
      case System::J2000:
         transform = glm::rotate( identity, glm::radians(0.0000275), glm::dvec3(0, 0, 1) ) *
         glm::rotate( identity, glm::radians(23.4392803055555555556), glm::dvec3(1, 0, 0) ) *
         glm::rotate( identity, glm::radians(180.0), glm::dvec3(0, 0, 1) ) *
         glm::rotate( identity, glm::radians(90.0), glm::dvec3(1, 0, 0) );
         break;
      case System::Galactic:
         break;
      default:
         break;
   }
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

template<class Archive> void Transform::serialize( Archive& ar, const unsigned int version ) {
	std::cout << "Serializing Transform" << std::endl;
	boost::serialization::void_cast_register<Transform, SceneObject>();
	ar & boost::serialization::make_nvp("SceneObject", boost::serialization::base_object<SceneObject>(*this));
	ar & BOOST_SERIALIZATION_NVP(transform);
}

