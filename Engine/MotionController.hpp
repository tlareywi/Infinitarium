//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "EventSampler.hpp"
#include "CoordinateSystem.hpp"
#include "SceneObject.hpp"

#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>
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
   virtual ~IMotionController() {
       eventSampler = nullptr;
       selectedObject = nullptr;
   }
   
   void processEvents( UpdateParams& );
   virtual void postUpdate() {};
   virtual void getCameraMatrix( glm::dmat4& );
   
   void getViewMatrix(glm::dmat4&);
   double getFOV() const {
       return cameraFOV;
   }
   
   void pushHome( const UniversalPoint& p );
   void popHome();
   bool onHomeStack(const UniversalPoint& p);
   UniversalPoint getHome() {
      return homeStack.back();
   }
   UniversalPoint childSystem(const UniversalPoint& p) const;
   
   virtual void select( const std::shared_ptr<SceneObject>& );
   virtual void lookAt( const std::shared_ptr<SceneObject>&, float duration ) {};
   virtual void track( const std::shared_ptr<SceneObject>& ) {};
   virtual void setAnchor( const std::shared_ptr<SceneObject>& obj ) {};

   virtual void animate(const glm::vec3& destPos, const glm::quat& destOrient, double ms);
   virtual void getViewComponents(glm::dvec3& eye, glm::dvec3& center, glm::dvec3& up, double& distance) const;
   glm::dmat4 localView(const UniversalPoint&);
   double setMovementMultiple( double m ) { 
       double oldMult = movementMultiple;
       movementMultiple = m; 
       return oldMult;
   }
   
protected:
   virtual void resetCenter(const glm::dvec3& pos) {}
   virtual void setDistance(double) {}
   virtual void setViewComponents(const glm::dvec3& eye, const glm::dvec3& center, const glm::dvec3& up);
   virtual void onKeyDown( const IEventSampler::Key& ) {}
   virtual void onKeyUp( const IEventSampler::Key& ) {}
   virtual void onMouseButtonDown( const IEventSampler::MouseButton& ) {}
   virtual void onMouseButtonUp( const IEventSampler::MouseButton& ) {}
   virtual void onMouseMove( const IEventSampler::MouseMove& ) {}
   virtual void onMouseScroll( const IEventSampler::MouseMove& ) {}
   virtual void onMouseDrag( const IEventSampler::MouseDrag& ) {}
   virtual void onMouseDoubleClick( const IEventSampler::MouseButton& ) {}
   virtual void onMouseButtonClick( const IEventSampler::MouseButton& ) {} 
   virtual void updateAnimation(double);
   
   glm::dmat4 cameraTransform;
   double cameraFOV{ 60.0 };
   double movementMultiple{ 1.0 };
   std::vector<UniversalPoint> homeStack;
   
   std::shared_ptr<IEventSampler> eventSampler;
   std::shared_ptr<SceneObject> selectedObject;

   glm::dvec3 sourcePosition;
   glm::dquat sourceOrientation;
   glm::dvec3 destPosition;
   glm::dquat destOrientation;
   double duration{ -1.0 };
   double elapsed{ 0.0 };
   
private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int);
};
