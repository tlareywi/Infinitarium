//
//  MotionControllerOrbit.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#pragma once

#include "MotionController.hpp"

#include "../config.h"

#include <boost/archive/polymorphic_xml_woarchive.hpp>
#include <boost/archive/polymorphic_xml_wiarchive.hpp>

#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

class Orbit : public IMotionController {
public:
   Orbit() : yawPitchRoll{1.0,0.0,0.0,0.0}, distance{1.0}, rotation(1.0,0.0,0.0,0.0), center(0.0,0.0,0.0), sensitivity(1.0f) {}
   virtual ~Orbit() {}
   
   void getViewMatrix( glm::mat4& ) override;
   
   void setAnchor( const std::shared_ptr<SceneObject>& obj );
   void animatePath();
      
protected:
   void onKeyDown( const IEventSampler::Key& ) override;
   void onMouseMove( const IEventSampler::MouseMove& ) override;
   void onMouseDrag( const IEventSampler::MouseDrag& ) override;
   void onMouseButtonClick( const IEventSampler::MouseButton& ) override;
   void onMouseDoubleClick( const IEventSampler::MouseButton& ) override;
   
private:
   inline float project( float r, float x, float y );
   void calculateAngleAxis( glm::dvec3& axis, float& angle, const glm::vec2&, const glm::vec2& );
   void rotateAboutAnchor( const glm::vec2&, const glm::vec2& );
   
   glm::dquat yawPitchRoll;
   double distance;
   glm::dquat rotation;
   glm::dvec3 center;
   
   float sensitivity;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      std::cout<<"Serializing Orbit MotionController"<<std::endl;
      boost::serialization::void_cast_register<Orbit,IMotionController>();
      ar & boost::serialization::make_nvp("IMotionController", boost::serialization::base_object<IMotionController>(*this));
   }
};

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY(IMotionController)
BOOST_CLASS_EXPORT_KEY(Orbit)
