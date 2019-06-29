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
#include <glm/gtc/quaternion.hpp>

#include "EventSampler.hpp"
#include "CoordinateSystem.hpp"
#include "SceneObject.hpp"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
   
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
   virtual void onMouseDoubleClick( const IEventSampler::MouseButtonDbl& ) {}

   glm::dmat4 view;
   std::vector<UniversalPoint> homeStack;
   
   std::shared_ptr<IEventSampler> eventSampler;
   std::shared_ptr<SceneObject> selectedObject;
   
private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int);
};


class Orbit : public IMotionController {
public:
   Orbit() : yawPitchRoll{1.0,0.0,0.0,0.0}, distance{1.0}, rotation(1.0,0.0,0.0,0.0), center(0.0,0.0,0.0) {}
   virtual ~Orbit() {}
   
   void getViewMatrix( glm::mat4& ) override;
   
   void setAnchor( const std::shared_ptr<SceneObject>& obj );
   void animatePath();
      
protected:
   void onKeyDown( const IEventSampler::Key& ) override;
   void onMouseMove( const IEventSampler::MouseMove& ) override;
   void onMouseDrag( const IEventSampler::MouseDrag& ) override;
   void onMouseButtonUp( const IEventSampler::MouseButton& ) override;
   void onMouseDoubleClick( const IEventSampler::MouseButtonDbl& ) override;
   
private:
   float projectToSphere( float r, float x, float y );
   void trackball( glm::dvec3& axis, float& angle, float p1x, float p1y, float p2x, float p2y );
   void rotateTrackball( float px0, float py0, float px1, float py1 );
   
   glm::dquat yawPitchRoll;
   double distance;
   glm::dquat rotation;
   glm::dvec3 center;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int);
};
