//
//  MotionControllerOrbit.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#include "MotionControllerOrbit.hpp"

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(IMotionController)
BOOST_CLASS_EXPORT(Orbit)

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
   
   for( auto& evt : eventSampler->mmove )
      onMouseMove( evt );
   
   for( auto& evt : eventSampler->mdrag )
      onMouseDrag( evt );
   
   eventSampler->clear();
}

void IMotionController::getViewMatrix( glm::mat4& out ) {
   out = view;
}


void Orbit::onKeyDown( const IEventSampler::Key& evt ) {
   
}
   
void Orbit::onMouseMove( const IEventSampler::MouseMove& evt ) {
   view = glm::rotate( view, glm::radians(evt.dx), glm::vec3(glm::vec4(0, 1, 0, 0) * view) );
   view = glm::rotate( view, glm::radians(evt.dy), glm::vec3(glm::vec4(1, 0, 0, 0) * view) );
}

void Orbit::onMouseDrag( const IEventSampler::MouseDrag& evt ) {
   view = glm::rotate( view, glm::radians(evt.dx), glm::vec3(glm::vec4(0, 1, 0, 0) * view) );
   view = glm::rotate( view, glm::radians(evt.dy), glm::vec3(glm::vec4(1, 0, 0, 0) * view) );
}


