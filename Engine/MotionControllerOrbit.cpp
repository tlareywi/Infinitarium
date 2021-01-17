//
//  MotionControllerOrbit.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#include "MotionControllerOrbit.hpp"
#include "Delegate.hpp"
#include "Application.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Orbit)

static std::mutex viewMutex{ std::mutex() };

void Orbit::animate(const glm::vec3& destDistance, const glm::quat& destYPR, double ms) {
    sourcePosition = glm::dvec3(0, 0, _distance);
    sourceOrientation = _yawPitchRoll;
    duration = ms;
    elapsed = 0.0;
    destPosition = destDistance;
    destOrientation = destYPR;
}

void Orbit::updateAnimation(double msTick) {
    static double rSum{ 0.0 };
    const double sigma{ 0.125 };

    if (elapsed < duration) {
        elapsed += msTick;

        // Acceleration curve for animation motion. 
        double frameWeight = (duration * (1.0 / msTick));
        rSum += 1.0 / (sigma * sqrt(glm::pi<double>() * 2.0)) * exp(-0.5 * pow((elapsed / duration - 0.5) / sigma, 2.0)) / frameWeight;

        glm::dquat q{ glm::slerp(sourceOrientation, destOrientation, rSum) };
        _yawPitchRoll = q;

        glm::dvec3 pos{ glm::mix(sourcePosition, destPosition, rSum) };
        _distance = pos.z;
    }
    else if (duration > 0) { // Ensure the last animation frame is processed
        duration = 0.0;
        rSum = 0.0;

        _yawPitchRoll = destOrientation;
        _distance = destPosition.z;
    }
}

void Orbit::setAnchor( const std::shared_ptr<SceneObject>& obj ) {
   resetCenter(obj->getCenter());
}

void Orbit::getViewComponents(glm::dvec3& eye, glm::dvec3& center, glm::dvec3& up) const {
    center = _center;
    eye = _center + _rotation * glm::dvec3(0., 0., _distance);
    up = _rotation * glm::dvec3(0., 1., 0.);
}

void Orbit::setViewComponents(const glm::dvec3& eye, const glm::dvec3& center, const glm::dvec3& up) {
    _center = center;
   
    glm::dvec3 forward{ center - eye };
    _distance = glm::length(forward);
    forward = glm::normalize(forward);
    glm::dvec3 side{ glm::normalize(glm::cross(forward, up)) };
    glm::dvec3 u{ glm::normalize(glm::cross(side, forward)) };

    glm::dmat4 rotation_matrix(1.0);
    rotation_matrix[0][0] = side.x;
    rotation_matrix[1][0] = side.y;
    rotation_matrix[2][0] = side.z;

    rotation_matrix[0][1] = u.x;
    rotation_matrix[1][1] = u.y;
    rotation_matrix[2][1] = u.z;

    rotation_matrix[0][2] = -forward.x;
    rotation_matrix[1][2] = -forward.y;
    rotation_matrix[2][2] = -forward.z;

    _rotation = glm::inverse(glm::toQuat(rotation_matrix));
    _yawPitchRoll = glm::quat(1.0, 0.0, 0.0, 0.0);
}

/// <summary>
/// Modifies the 'center' component of the view matrix while keeping the composite transform of the view matrix equal.
/// The yawPitchRoll rotation is modified such that moving 'center' does not change the result of the view matrix. 
/// </summary>
/// <param name="pos">New center for view matrix</param>
void Orbit::resetCenter(const glm::dvec3& pos) {
    glm::dmat4 oldView;
    getCameraMatrix(oldView);
    oldView = glm::inverse(oldView);

    glm::dvec3 eye, center, up;
    getViewComponents(eye, center, up);
    setViewComponents(eye, pos, up);

    // Derive rotation needed to make the new view matrix equivalent to the old one but with the new composition.
    glm::dmat4 mat;
    getCameraMatrix(mat);
    glm::dmat4 rotation{ oldView * mat };
    glm::quat q{ glm::toQuat(rotation) };
    _yawPitchRoll = glm::inverse(q);
}

void Orbit::onKeyDown( const IEventSampler::Key& evt ) {
   switch( evt.key ) {
      case 'a': {
         rotateAboutAnchor(glm::vec2(0.0f,0.0f), glm::vec2(-0.01f, 0.0f));
         break;
      }
      case 'd': {
         rotateAboutAnchor(glm::vec2(0.0f,0.0f), glm::vec2(0.01f, 0.0f));
         break;
      }
      case 'w': {
         _distance -= 0.4;
         break;
      }
      case 's': {
         _distance += 0.4;
         break;
      }
      case 256: { //ESC
          std::tuple<int> args(evt.key);
          Event e( args );
          e.setName("ESC");
          IApplication::Create()->invoke(e);
          break;
      }
      default:
         break;
   }
}

void Orbit::rotateAboutAnchor( const glm::vec2& p1, const glm::vec2& p2 ) {
   glm::dvec3 axis;
   float angle;
   
   calculateAngleAxis( axis, angle, glm::vec2(p1.x + (p2.x-p1.x), p1.y + (p2.y-p1.y)), p1 );
   
   glm::dquat new_rotate(1.0,0.0,0.0,0.0);
   new_rotate = glm::rotate( new_rotate, (double)angle, axis );
   
   _rotation = new_rotate * _rotation;
}

void Orbit::calculateAngleAxis( glm::dvec3& axis, float& angle, const glm::vec2& p1, const glm::vec2& p2 ) {
   glm::dmat4 rotation_matrix{ glm::toMat4(_rotation) };
   
   glm::dvec3 up{ rotation_matrix * glm::dvec4(0.0,1.0,0.0,0.0) };
   glm::dvec3 side{ rotation_matrix * glm::dvec4(1.0,0.0,0.0,0.0) };
   glm::dvec3 front{ rotation_matrix * glm::dvec4(0.0,0.0,-1.0,0.0) };
   
   // Could use 'distance' instead of a separate sensitivity if we want a consistent angular distance. That may make sense for certain animations
   // but setting a constant sensitivity for now.
   glm::dvec3 pp1 = side * (double)p1.x + up * (double)p1.y - front;
   glm::dvec3 pp2 = side * (double)p2.x + up * (double)p2.y - front;

   axis = glm::cross(pp2, pp1);
   axis = glm::normalize(axis);
   
   // Angle between p1 and p2 at distance/sensitivity.
   float t = glm::length(pp2 - pp1);
   t = std::clamp(t, -1.0f, 1.0f);
   angle = glm::radians(asin(t));
}

void Orbit::onMouseButtonClick( const IEventSampler::MouseButton& evt ) {
   // Fire picking event
   const glm::uvec2 pt(evt.x, evt.y);
   std::tuple<const glm::uvec2&> args( pt );
   Event e( args );
   e.setName("picking");
   IApplication::Create()->invoke( e );
}

void Orbit::onMouseDoubleClick( const IEventSampler::MouseButton& ) {
   if( !selectedObject )
      return;
   
   setAnchor( selectedObject );

   glm::dvec3 eye, center, up;
   getViewComponents(eye, center, up);
   animate( glm::dvec3(0,0,5.0), glm::dquat(1,0,0,0), 5000.0 );
}
   
void Orbit::onMouseMove( const IEventSampler::MouseMove& evt ) {
   
}

void Orbit::onMouseDrag( const IEventSampler::MouseDrag& evt ) {
    if (evt.button == IEventSampler::Button::LEFT) {
        _yawPitchRoll = glm::rotate(_yawPitchRoll, glm::radians((double)-evt.dx), glm::dvec3(0.0, 1.0, 0.0));
        _yawPitchRoll = glm::rotate(_yawPitchRoll, glm::radians((double)-evt.dy), glm::dvec3(1.0, 0.0, 0.0));
    }
    else if (evt.button == IEventSampler::Button::MIDDLE) {
        _distance -= (double)evt.dy;
    }
    else if (evt.button == IEventSampler::Button::RIGHT) {
        rotateAboutAnchor(glm::vec2(0.0f, 0.0f), glm::vec2(-evt.dx, evt.dy));
    }
}

void Orbit::getCameraMatrix( glm::dmat4& out ) {
   glm::dmat4 identity(1.0);
 
   std::scoped_lock<std::mutex> lock {viewMutex};
   
   cameraTransform =
       glm::translate(identity, _center) *
       glm::toMat4(_rotation) *
       glm::translate(identity, glm::dvec3(0.0, 0.0, _distance)) *
       glm::toMat4( _yawPitchRoll ); // Column major, so first transform in stack.
   
   out = cameraTransform;
}

template<class Archive> void Orbit::serialize(Archive & ar, const unsigned int version) {
	std::cout << "Serializing Orbit MotionController" << std::endl;
	boost::serialization::void_cast_register<Orbit, IMotionController>();
	ar& boost::serialization::make_nvp("IMotionController", boost::serialization::base_object<IMotionController>(*this));
}


