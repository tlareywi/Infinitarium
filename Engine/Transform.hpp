//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "SceneObject.hpp"
#include "../config.h"

#include "../mechanics/OrbitalFactory.hpp"

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <functional>

class Transform : public SceneObject {
public:
   enum class System {
      VSOP87,
      J2000,
      Galactic
   };
   
   Transform();
   virtual ~Transform() {}
   
   void update( UpdateParams& ) override;
   
   void setMatrix( const glm::mat4& );
   void setTransform( System );
   glm::mat4x4 getMatrix();
   
   void identity();
   void scale( float x, float y, float z );
   void translate( float x, float y, float z );
   void applyTranslate( const glm::dvec3& );
   void rotate( float angle, float x, float y, float z );
   
   void setPositionCallback( OrbitalFactory::PositionCallback id ) {
       positionCallbackId = id;
   }
    
   void setUpVector( const glm::dvec3& u ) {
       up = u;
   }
    
   void setRotateAboutUpRate( double r ) {
       rotationRate = r;
   }
    
   template<class Archive> void save( Archive& ) const;
   template<class Archive> void load( Archive& );
   
private:
   void rotateInternal( float radians, const glm::vec3& axis );
    
   glm::mat4x4 transform;
    
   OrbitalFactory::PositionCallback positionCallbackId;
   OrbitalFactory::PositionCallbackFun positionCallback;
   
   glm::dvec3 up;
   double rotationRate;
    
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive&, const unsigned int);
};

BOOST_CLASS_EXPORT_KEY(Transform)

