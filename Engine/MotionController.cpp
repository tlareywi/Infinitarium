//
//  MotionController.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 7/13/19.
//

#include "MotionController.hpp"
#include "Scene.hpp"
#include "Stats.hpp"

#include <glm/gtx/matrix_decompose.hpp>

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_IMPLEMENT(IMotionController)

IMotionController::IMotionController() {
   cameraTransform = glm::lookAt( glm::vec3(0,0,0), glm::vec3(0,0,1), glm::vec3(0,1,0) );
   eventSampler = IEventSampler::Create();
}

void IMotionController::animate(const glm::vec3& destPos, const glm::quat& destOrient, double ms) {
    glm::dvec3 scale;
    glm::dquat rotation;
    glm::dvec3 translation;
    glm::dvec3 skew;
    glm::dvec4 perspective;
    glm::dmat4 mat;
    getCameraMatrix(mat);
    glm::decompose(mat, scale, rotation, translation, skew, perspective);
    
    sourcePosition = translation;
    sourceOrientation = rotation;
    duration = ms;
    elapsed = 0.0;
    destPosition = destPos;
    destOrientation = destOrient;
}

void IMotionController::updateAnimation( double msTick ) {
    if (elapsed < duration) {
        glm::dmat4 position(1.0);

        elapsed += msTick;
        glm::dquat q{ glm::slerp(sourceOrientation, destOrientation, elapsed / duration) };
        glm::dmat4 orientation{ glm::toMat4(q) };

        glm::dvec3 pos{ glm::mix(sourcePosition, destPosition, elapsed / duration) };
        position = glm::translate(position, pos);

        cameraTransform = orientation * position;
    }
    else if (duration > 0) { // Ensure the last animation frame is processed
        duration = 0.0;
        glm::dmat4 position(1.0);

        glm::dmat4 orientation{ glm::toMat4(destOrientation) };
        position = glm::translate(position, destPosition);

        cameraTransform = orientation * position;
    }
}

void IMotionController::getViewComponents(glm::dvec3& eye, glm::dvec3& center, glm::dvec3& up) const {
    center = glm::dvec3(0,0,-1);
    eye = cameraTransform * glm::dvec4(0, 0, 0, 1);
    up = glm::dvec3(0, 1, 0);
}

void IMotionController::processEvents( UpdateParams& params ) {
   std::lock_guard<std::mutex> lock(eventSampler->eventMutex);
   
   for( auto& evt : eventSampler->keys ) {
      if( evt.state == IEventSampler::State::DOWN )
         onKeyDown( evt );
      else if( evt.state == IEventSampler::State::UP )
         onKeyUp( evt );
   }
   
   for( auto& evt : eventSampler->mbutton ) {
      if( evt.state == IEventSampler::State::DOWN )
         onMouseButtonDown( evt );
      else if( evt.state == IEventSampler::State::UP )
         onMouseButtonUp( evt );
      else if( evt.state == IEventSampler::State::CLICKED )
         onMouseButtonClick( evt );
      else if( evt.state == IEventSampler::State::DBL_CLICKED )
         onMouseDoubleClick( evt );
   }
   
   for (auto& evt : eventSampler->mmove) {
       if (fabs(evt.dz) > 0)
           onMouseScroll(evt);
       else
           onMouseMove(evt);
   }
   
   for( auto& evt : eventSampler->mdrag )
      onMouseDrag( evt );
   
   eventSampler->clear();

   updateAnimation( params.getScene().tickTime().count() );

   Stats& stats{ Stats::Instance() };
   getViewComponents( stats.eye, stats.center, stats.up );
}

void IMotionController::pushHome( const UniversalPoint& p ) {
   if( !homeStack.empty() ) {
      glm::dvec3 eye = cameraTransform[3];
      UniversalPoint camera{ eye.x, eye.y, eye.z, getHome().getUnit() };
      UniversalPoint localEye = camera.convert( p.getUnit() );
      cameraTransform[3] = glm::dvec4( localEye.getPoint(), 1.0 );
   }
   
   homeStack.push_back( p );
}
void IMotionController::popHome() {
   if( homeStack.empty() )
      return;
   
   glm::dvec3 eye = cameraTransform[3];
   UniversalPoint camera{ eye.x, eye.y, eye.z, getHome().getUnit() };
   homeStack.pop_back();
   
   UniversalPoint localEye = camera.convert( getHome().getUnit() );
   cameraTransform[3] = glm::dvec4( localEye.getPoint(), 1.0 );
}

void IMotionController::getCameraMatrix( glm::dmat4& out ) {
   out = cameraTransform;
}

void IMotionController::getViewMatrix( glm::dmat4& out ) {
    getCameraMatrix(out);
    out = glm::inverse(out);
}

void IMotionController::select( const std::shared_ptr<SceneObject>& obj ) {
   selectedObject = obj;
}

template<class Archive> void IMotionController::serialize(Archive & ar, const unsigned int version) {
   std::cout<<"Serializing IMotionController"<<std::endl;
   ar & BOOST_SERIALIZATION_NVP(homeStack);
}
