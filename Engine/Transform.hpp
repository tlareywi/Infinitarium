//
//  Transform.hpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 3/18/19.
//

#pragma once

#include "SceneObject.hpp"

#include <glm/glm.hpp>

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
   void rotate( float angle, float x, float y, float z );
   
private:
   glm::mat4x4 transform;
   
   friend class boost::serialization::access;
   template<class Archive> friend void boost::serialization::serialize( Archive&, Transform&, unsigned int );
};

