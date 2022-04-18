//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "SceneObject.hpp"
#include "../config.h"

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

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
   
private:
   glm::mat4x4 transform;

   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive&, const unsigned int);
};

BOOST_CLASS_EXPORT_KEY(Transform)

