//
//  MotionController.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 7/13/19.
//

#include "MotionController.hpp"

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_IMPLEMENT(IMotionController)

IMotionController::IMotionController() {
   view = glm::lookAt( glm::vec3(0,0,0), glm::vec3(0,0,1), glm::vec3(0,1,0) );
   eventSampler = IEventSampler::Create();
}

void IMotionController::processEvents() {
   std::lock_guard<std::mutex> lock(eventSampler->eventMutex);
   
   for( auto& evt : eventSampler->keys ) {
      if( evt.state == IEventSampler::DOWN )
         onKeyDown( evt );
      else if( evt.state == IEventSampler::UP )
         onKeyUp( evt );
   }
   
   for( auto& evt : eventSampler->mbutton ) {
      if( evt.state == IEventSampler::DOWN )
         onMouseButtonDown( evt );
      else if( evt.state == IEventSampler::UP )
         onMouseButtonUp( evt );
      else if( evt.state == IEventSampler::CLICKED )
         onMouseButtonClick( evt );
      else if( evt.state == IEventSampler::DBL_CLICKED )
         onMouseDoubleClick( evt );
   }
   
   for( auto& evt : eventSampler->mmove )
      onMouseMove( evt );
   
   for( auto& evt : eventSampler->mdrag )
      onMouseDrag( evt );
   
   eventSampler->clear();
}

void IMotionController::pushHome( const UniversalPoint& p ) {
   if( !homeStack.empty() ) {
      glm::dvec3 eye = view[3];
      UniversalPoint camera{ eye.x, eye.y, eye.z, getHome().getUnit() };
      UniversalPoint localEye = camera.convert( p.getUnit() );
      view[3] = glm::dvec4( localEye.getPoint(), 1.0 );
   }
   
   homeStack.push_back( p );
}
void IMotionController::popHome() {
   if( homeStack.empty() )
      return;
   
   glm::dvec3 eye = view[3];
   UniversalPoint camera{ eye.x, eye.y, eye.z, getHome().getUnit() };
   homeStack.pop_back();
   
   UniversalPoint localEye = camera.convert( getHome().getUnit() );
   view[3] = glm::dvec4( localEye.getPoint(), 1.0 );
}

void IMotionController::getViewMatrix( glm::mat4& out ) {
   out = view;
}

void IMotionController::select( const std::shared_ptr<SceneObject>& obj ) {
   selectedObject = obj;
}

template<class Archive> void IMotionController::serialize(Archive & ar, const unsigned int version) {
   std::cout<<"Serializing IMotionController"<<std::endl;
   ar & BOOST_SERIALIZATION_NVP(homeStack);
}
