//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
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

glm::dmat4 IMotionController::localView(const UniversalPoint& reference) {
    glm::dmat4 localView(1.0);
    glm::dmat4 identity(1.0);

    glm::dvec3 scale;
    glm::dquat rotation;
    glm::dvec3 translation;
    glm::dvec3 skew;
    glm::dvec4 perspective;
    glm::dmat4 mat;
    getCameraMatrix(mat);
    glm::decompose(mat, scale, rotation, translation, skew, perspective);

    localView = glm::translate(identity, reference.getPoint()) * glm::toMat4(rotation);

    return glm::inverse(localView);
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

void IMotionController::getViewComponents(glm::dvec3& eye, glm::dvec3& center, glm::dvec3& up, double& distance ) const {
    // TODO: incomplete implementation
    center = glm::dvec3(0,0,-1);
    eye = cameraTransform * glm::dvec4(0, 0, 0, 1);
    up = glm::dvec3(0, 1, 0);
    distance = 1.0;
}

void IMotionController::setViewComponents(const glm::dvec3& eye, const glm::dvec3& center, const glm::dvec3& up) {
    cameraTransform = glm::lookAt(eye, center, up);
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
   getViewComponents( stats.eye, stats.center, stats.up, stats.distance );
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

/// <summary> ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Coordinate system switching support methods
/// </summary> ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void IMotionController::pushHome(const UniversalPoint& p) {
    if (!homeStack.empty()) { // No current system to convert from.
        glm::dvec3 eye, center, up;
        double distance;
        getViewComponents(eye, center, up, distance);

        UniversalPoint d{ distance, distance, distance, getHome().getUnit() };
        UniversalPoint localDist = d.convert(p.getUnit());

        setDistance(localDist.getPoint().x);

        resetCenter(glm::dvec3(0.0, 0.0, 0.0));
    }

    homeStack.push_back(p);
}

void IMotionController::popHome() {
    if (homeStack.empty())
        return;

    glm::dvec3 eye, center, up;
    double distance;
    getViewComponents(eye, center, up, distance);

    UniversalPoint pt = homeStack.back();

    UniversalPoint d{ distance, distance, distance, getHome().getUnit() };
    homeStack.pop_back();
    UniversalPoint localDist = d.convert(getHome().getUnit());

    setDistance(localDist.getPoint().x);

    resetCenter(pt.getPoint());
}

bool IMotionController::onHomeStack(const UniversalPoint& p) {
    for (auto& point : homeStack) {
        if (point == p)
            return true;
    }

    return false;
}

UniversalPoint IMotionController::childSystem(const UniversalPoint& p) const {
    bool next{ false };
    for (auto& point : homeStack) {
        if (next)
            return point;

        if (point == p)
            next = true;
    }

    return p;
}
/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive> void IMotionController::serialize(Archive & ar, const unsigned int version) {
   std::cout<<"Serializing IMotionController"<<std::endl;
   ar & BOOST_SERIALIZATION_NVP(homeStack);
}
