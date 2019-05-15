//
//  MotionControllerOrbit.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#pragma once

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "EventSampler.hpp"
#include "CoordinateSystem.hpp"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
   
class IMotionController {
public:
   IMotionController();
   virtual ~IMotionController() {}
      
   void processEvents();
   void getViewMatrix( glm::mat4& );
   
   void pushHome( const UniversalPoint& p ) {
      homeStack.push_back( p );
   }
   void popHome() {
      homeStack.pop_back();
   }
   UniversalPoint getHome() {
      return homeStack.back();
   }
      
protected:
   virtual void onKeyDown( const IEventSampler::Key& ) {}
   virtual void onKeyUp( const IEventSampler::Key& ) {}
   virtual void onMouseButtonDown( const IEventSampler::MouseButton& ) {}
   virtual void onMouseButtonUp( const IEventSampler::MouseButton& ) {}
   virtual void onMouseMove( const IEventSampler::MouseMove& ) {}
   virtual void onMouseScroll( const IEventSampler::MouseMove& ) {}
   virtual void onMouseDrag( const IEventSampler::MouseDrag& ) {}

   glm::mat4 view;
   std::vector<UniversalPoint> homeStack;
   
   std::shared_ptr<IEventSampler> eventSampler;
   
private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int);
};


class Orbit : public IMotionController {
public:
   Orbit() {}
   virtual ~Orbit() {}
      
protected:
   void onKeyDown( const IEventSampler::Key& ) override;
   void onMouseMove( const IEventSampler::MouseMove& ) override;
   void onMouseDrag( const IEventSampler::MouseDrag& ) override;
   void onMouseButtonUp( const IEventSampler::MouseButton& ) override;
   
private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int);
};
