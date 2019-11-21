//
//  MotionControllerOrbit.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#pragma once

#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "EventSampler.hpp"
#include "CoordinateSystem.hpp"
#include "SceneObject.hpp"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

class KeyFrame {
public:
   KeyFrame();
   
private:
   glm::dquat yawPitchRoll;
   double distance;
   glm::dquat rotation;
   glm::dvec3 center;
};

class IMotionController {
public:
   IMotionController();
   virtual ~IMotionController() {}
   
   void processEvents();
   virtual void getViewMatrix( glm::mat4& );
   
   void pushHome( const UniversalPoint& p );
   void popHome();
   UniversalPoint getHome() {
      return homeStack.back();
   }
   
   void select( const std::shared_ptr<SceneObject>& );
   
protected:
   virtual void onKeyDown( const IEventSampler::Key& ) {}
   virtual void onKeyUp( const IEventSampler::Key& ) {}
   virtual void onMouseButtonDown( const IEventSampler::MouseButton& ) {}
   virtual void onMouseButtonUp( const IEventSampler::MouseButton& ) {}
   virtual void onMouseMove( const IEventSampler::MouseMove& ) {}
   virtual void onMouseScroll( const IEventSampler::MouseMove& ) {}
   virtual void onMouseDrag( const IEventSampler::MouseDrag& ) {}
   virtual void onMouseDoubleClick( const IEventSampler::MouseButton& ) {}
   virtual void onMouseButtonClick( const IEventSampler::MouseButton& ) {}
   
   glm::dmat4 view;
   std::vector<UniversalPoint> homeStack;
   
   std::shared_ptr<IEventSampler> eventSampler;
   std::shared_ptr<SceneObject> selectedObject;
   
private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int);
};
