//
//  MotionControllerOrbit.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#include "MotionControllerOrbit.hpp"
#include "Delegate.hpp"
#include "Application.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

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

void Orbit::setAnchor( const std::shared_ptr<SceneObject>& obj ) {
   center = obj->getCenter();
   
   glm::dvec3 eye(0.0,0.0,0.0);
   glm::dvec3 up(0.0,1.0,0.0);
   
   glm::dvec3 front{ center - eye };
   distance = glm::length( front );
   front = glm::normalize( front );
   glm::dvec3 side{ glm::cross(front, up) };
   side = glm::normalize( side );
   glm::dvec3 u{ glm::cross(side, front) };
   
   glm::dmat4 rotation_matrix(1.0);
   rotation_matrix[0][0] = side.x;
   rotation_matrix[1][0] = side.y;
   rotation_matrix[2][0] = side.z;
   
   rotation_matrix[0][1] = u.x;
   rotation_matrix[1][1] = u.y;
   rotation_matrix[2][1] = u.z;
   
   front = -front;
   rotation_matrix[0][2] = front.x;
   rotation_matrix[1][2] = front.y;
   rotation_matrix[2][2] = front.z;
   
   rotation = glm::toQuat(rotation_matrix);
   yawPitchRoll = glm::quat(1.0,0.0,0.0,0.0);
}

void Orbit::animatePath() {
   glm::dvec3 eye = view * glm::dvec4(0.0,0.0,0.0,1.0);
   glm::dvec3 center = view[2];
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
 //  glm::dmat4 preRotate{ glm::toMat4(yawPitchRoll) };
 //  glm::dvec3 a{ glm::dvec4(0.0,1.0,0.0,0.0) * preRotate };
 //  yawPitchRoll = glm::rotate( yawPitchRoll, glm::radians((double)evt.dx), a );
 //  glm::dvec3 b{ glm::dvec4(1.0,0.0,0.0,0.0) * preRotate };
 //  yawPitchRoll = glm::rotate( yawPitchRoll, glm::radians((double)evt.dy), b );
}

void Orbit::onMouseDrag( const IEventSampler::MouseDrag& evt ) {
   glm::dmat4 preRotate{ glm::toMat4(yawPitchRoll) };
   glm::dvec3 a{ glm::dvec4(0.0,1.0,0.0,0.0) * preRotate };
   yawPitchRoll = glm::rotate( yawPitchRoll, glm::radians((double)evt.dx), a );
   glm::dvec3 b{ glm::dvec4(1.0,0.0,0.0,0.0) * preRotate };
   yawPitchRoll = glm::rotate( yawPitchRoll, glm::radians((double)evt.dy), b );
}

void Orbit::getViewMatrix( glm::mat4& out ) {
   glm::dmat4 identity(1.0);
   
  view =
   glm::toMat4( yawPitchRoll ) *
   glm::translate( identity, glm::dvec3(0.0,0.0,distance) ) *
   glm::toMat4( rotation ) *
   glm::translate( identity, center );
   
   out = view;
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


