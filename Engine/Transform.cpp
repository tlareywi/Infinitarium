//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "Transform.hpp"
#include "UniformType.hpp"

#include "Scene.hpp"

#include "OrbitalFactory.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Transform)

static constexpr double HoursToMilliseconds = 1000.0 * 60.0 * 60.0;

Transform::Transform() : transform(glm::mat4(1.0)),
    positionCallbackId(OrbitalFactory::PositionCallback::None),
    up(glm::dvec3(0.0,1.0,0.0)),
    rotationRate(0.0) {
}

void Transform::update( UpdateParams& params ) {
   if( positionCallback ) {
       glm::dvec3 pos;
       positionCallback(params.getScene().JD(), pos);
       transform[3][0] = pos.x;
       transform[3][1] = pos.y;
       transform[3][2] = pos.z;
       transform[3][3] = 1.0;
      // std::cout << pos.x << " " << pos.y << " " << pos.z << std::endl;
   }
    
   if( rotationRate ) // In hours per full rotation (sidereal)
      rotateInternal( params.getScene().simulationTimeDelta().count() / (rotationRate * HoursToMilliseconds) * 2.0*M_PI, up );
    
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

void Transform::applyTranslate(const glm::dvec3& point) {
    glm::vec3 fvec(point.x, point.y, point.z);
    transform = glm::translate(glm::mat4(1.0), fvec) * transform;
}

void Transform::rotate( float degrees, float x, float y, float z ) {
   transform = glm::rotate( glm::mat4(1.0), glm::radians(degrees), glm::vec3(x, y, z)) * transform;
}

void Transform::rotateInternal( float radians, const glm::vec3& axis ) {
   transform = glm::rotate( glm::mat4(1.0), radians, axis) * transform;
}

//////////////////////////////////////////////////////////////////////////////////////////
// SERIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////
template<class Archive> void Transform::load( Archive& ar ) {
    ar >> positionCallbackId;
    ar >> transform;
    ar >> up;
    ar >> rotationRate;
    
    // Instance our position callback if defined
    if( positionCallbackId != OrbitalFactory::PositionCallback::None )
        positionCallback = OrbitalFactory::instance().orbitalSampler(positionCallbackId);
}

template<class Archive> void Transform::save( Archive& ar ) const {
    ar << positionCallbackId;
    ar << transform;
    ar << up;
    ar << rotationRate;
}

namespace boost { namespace serialization {
   template<class Archive> inline void load(Archive& ar, Transform& t, unsigned int version) {
      t.load( ar );
      std::cout << "Loading Transform " << t.getName() << std::endl;
   }
   template<class Archive> inline void save(Archive& ar, const Transform& t, const unsigned int version) {
      std::cout<<"Saving Transform "<< t.getName() << std::endl;
      t.save( ar );
   }
}}

template<class Archive> void Transform::serialize( Archive& ar, const unsigned int version ) {
	boost::serialization::void_cast_register<Transform, SceneObject>();
    boost::serialization::split_free(ar, *this, version);
	ar & boost::serialization::base_object<SceneObject>(*this);
}

