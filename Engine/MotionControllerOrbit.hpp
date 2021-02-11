//
//  MotionControllerOrbit.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#pragma once

#include "MotionController.hpp"

#include "../config.h"

#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>

class Orbit : public IMotionController {
public:
   Orbit() : _yawPitchRoll{1.0,0.0,0.0,0.0}, _distance{0.0}, _rotation(1.0,0.0,0.0,0.0), _center(0.0,0.0,0.0) {}
   virtual ~Orbit() {}
   
   void getCameraMatrix( glm::dmat4& ) override;
   
   void setAnchor( const std::shared_ptr<SceneObject>& obj );

   void animate(const glm::vec3&, const glm::quat&, double ) override;
   void getViewComponents(glm::dvec3&, glm::dvec3&, glm::dvec3&) const override;
      
protected:
   void onKeyDown( const IEventSampler::Key& ) override;
   void onMouseMove( const IEventSampler::MouseMove& ) override;
   void onMouseDrag( const IEventSampler::MouseDrag& ) override;
   void onMouseButtonClick( const IEventSampler::MouseButton& ) override;
   void onMouseDoubleClick( const IEventSampler::MouseButton& ) override;
   void updateAnimation( double ) override;
   
private:
   void setViewComponents(const glm::dvec3&, const glm::dvec3&, const glm::dvec3&);
   void resetCenter(const glm::dvec3& pos);
   void calculateAngleAxis( glm::dvec3& axis, float& angle, const glm::vec2&, const glm::vec2& );
   void rotateAboutAnchor( const glm::vec2&, const glm::vec2& );
   
   glm::dquat _yawPitchRoll;
   double _distance;
   glm::dquat _rotation;
   glm::dvec3 _center;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive& ar, const unsigned int version);
};

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY(IMotionController)
BOOST_CLASS_EXPORT_KEY(Orbit)
