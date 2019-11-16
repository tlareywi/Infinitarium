//
//  Transform.hpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 3/18/19.
//

#pragma once

#include "SceneObject.hpp"
#include "../config.h"

#include <glm/glm.hpp>

class IE_EXPORT Transform : public SceneObject {
public:
   Transform();
   virtual ~Transform() {}
   
   void update( const glm::mat4& ) override;
   
   void setMatrix( const glm::mat4& );
   glm::mat4x4 getMatrix();
   
private:
   glm::mat4x4 transform;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive&, const unsigned int);
};
