//
//  MotionControllerOrbit.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#include "MotionControllerOrbit.hpp"
#include "Delegate.hpp"
#include "Application.hpp"

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
   
   for( auto& evt : eventSampler->mbutton ) {
      if( evt.state == IEventSampler::DOWN )
         onMouseButtonDown( evt );
      else if( evt.state == IEventSampler::UP )
         onMouseButtonUp( evt );
   }
   
   for( auto& evt : eventSampler->mbuttonDbl )
         onMouseDoubleClick( evt );
   
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

void IMotionController::setAnchor( const std::shared_ptr<SceneObject>& obj ) {
   // view[2] -> center component of view matrix.
  // view[2] = glm::vec4(obj->getCenter(), view[2][3]);
   std::cout<<"Center of system "<<obj->getCenter().x<<" "<<obj->getCenter().y<<" "<<obj->getCenter().z<<std::endl;
   view = glm::lookAt(glm::vec3(0.0,0.0,0.0), obj->getCenter(), glm::vec3(0.0,1.0,0.0));
   
   // TODO: add rotation to retain the appearance of not moving the center.
}

void IMotionController::animatePath() {
   glm::vec3 eye = view[3];
   glm::vec3 center = view[2];
   
   double distance = glm::distance(eye, center);
   std::cout<<"Distance to system "<<distance<<std::endl;
}

void Orbit::onKeyDown( const IEventSampler::Key& evt ) {
   
}

void Orbit::onMouseButtonUp( const IEventSampler::MouseButton& evt ) {
   // Fire picking event
   const glm::uvec2 pt(evt.x, evt.y);
   std::tuple<const glm::uvec2&> args( pt );
   Event e( args );
   e.setName("picking");
   IApplication::Create()->invoke( e );
}

void Orbit::onMouseDoubleClick( const IEventSampler::MouseButtonDbl& ) {
   if( !selectedObject )
      return;
   
   setAnchor( selectedObject );
   animatePath();
}
   
void Orbit::onMouseMove( const IEventSampler::MouseMove& evt ) {
   view = glm::rotate( view, glm::radians(evt.dx), glm::vec3(glm::vec4(0, 1, 0, 0) * view) );
   view = glm::rotate( view, glm::radians(evt.dy), glm::vec3(glm::vec4(1, 0, 0, 0) * view) );
}

void Orbit::onMouseDrag( const IEventSampler::MouseDrag& evt ) {
   view = glm::rotate( view, glm::radians(evt.dx), glm::vec3(glm::vec4(0, 1, 0, 0) * view) );
   view = glm::rotate( view, glm::radians(evt.dy), glm::vec3(glm::vec4(1, 0, 0, 0) * view) );
}

template<class Archive> void IMotionController::serialize(Archive & ar, const unsigned int version) {
   std::cout<<"Serializing IMotionController"<<std::endl;
   ar & homeStack;
}

template<class Archive> void Orbit::serialize(Archive & ar, const unsigned int version) {
   std::cout<<"Serializing Orbit MotionController"<<std::endl;
   boost::serialization::void_cast_register<Orbit,IMotionController>();
   ar & boost::serialization::base_object<IMotionController>(*this);
}


