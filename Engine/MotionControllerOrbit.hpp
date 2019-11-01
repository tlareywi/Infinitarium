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

#include <boost/archive/xml_woarchive.hpp>
#include <boost/archive/xml_wiarchive.hpp>
   
class IMotionController {
public:
   IMotionController();
   virtual ~IMotionController() {}
      
   void processEvents();
   void getViewMatrix( glm::mat4& );
      
protected:
   virtual void onKeyDown( const IEventSampler::Key& ) {}
   virtual void onKeyUp( const IEventSampler::Key& ) {}
   virtual void onMouseButtonDown( const IEventSampler::MouseButton& ) {}
   virtual void onMouseButtonUp( const IEventSampler::MouseButton& ) {}
   virtual void onMouseMove( const IEventSampler::MouseMove& ) {}
   virtual void onMouseScroll( const IEventSampler::MouseMove& ) {}
   virtual void onMouseDrag( const IEventSampler::MouseDrag& ) {}

   glm::mat4 view;
   
   std::shared_ptr<IEventSampler> eventSampler;
   
private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      std::cout<<"Serializing IMotionController"<<std::endl;
   }
};


class Orbit : public IMotionController {
public:
   Orbit() {}
   virtual ~Orbit() {}
      
protected:
   void onKeyDown( const IEventSampler::Key& ) override;
   void onMouseMove( const IEventSampler::MouseMove& ) override;
   void onMouseDrag( const IEventSampler::MouseDrag& ) override;
   
private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      std::cout<<"Serializing Orbit MotionController"<<std::endl;
      boost::serialization::void_cast_register<Orbit,IMotionController>();
      ar & BOOST_SERIALIZATION_NVP(boost::serialization::base_object<IMotionController>(*this));
   }
};
