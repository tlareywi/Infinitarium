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

void Transform::update( const glm::mat4& mat ) {
   SceneObject::update( transform * mat );
}

void Transform::setMatrix( const glm::mat4& mat ) {
   transform = mat;
}

glm::mat4x4 Transform::getMatrix() {
   return transform;
}

template<class Archive> void Transform::serialize( Archive& ar, const unsigned int version ) {
	std::cout << "Serializing Transform" << std::endl;
	boost::serialization::void_cast_register<Transform, SceneObject>();
	ar & boost::serialization::make_nvp("SceneObject", boost::serialization::base_object<SceneObject>(*this));
	ar & BOOST_SERIALIZATION_NVP(transform);
}


