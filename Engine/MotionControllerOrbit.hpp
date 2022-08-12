//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "MotionController.hpp"

#include "../config.h"

#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>

class Orbit : public IMotionController {
public:
   Orbit() : _yawPitchRoll{1.0,0.0,0.0,0.0}, _distance{0.000001}, _rotation(1.0,0.0,0.0,0.0), _center(0.0,0.0,0.0), tracking{ nullptr } {}
   virtual ~Orbit() {}
    
   void postUpdate() override;
   
   void getCameraMatrix( glm::dmat4& ) override;
   
   void setAnchor( const std::shared_ptr<SceneObject>& obj ) override;
   void lookAt( const std::shared_ptr<SceneObject>& obj, float duration ) override;
   void track( const std::shared_ptr<SceneObject>& obj ) override;

   void animate(const glm::vec3&, const glm::quat&, double ) override;
   void getViewComponents(glm::dvec3&, glm::dvec3&, glm::dvec3&, double& distance) const override;
      
protected:
   void resetCenter(const glm::dvec3& pos) override;
   void setDistance(double d) override { _distance = d; }
   void setViewComponents(const glm::dvec3&, const glm::dvec3&, const glm::dvec3&) override;
   void onKeyDown( const IEventSampler::Key& ) override;
   void onMouseMove( const IEventSampler::MouseMove& ) override;
   void onMouseDrag( const IEventSampler::MouseDrag& ) override;
   void onMouseScroll(const IEventSampler::MouseMove&) override;
   void onMouseButtonClick( const IEventSampler::MouseButton& ) override;
   void onMouseDoubleClick( const IEventSampler::MouseButton& ) override;
   void updateAnimation( double ) override;
   
private:
   void calculateAngleAxis( glm::dvec3& axis, float& angle, const glm::vec2&, const glm::vec2& );
   void rotateAboutAnchor( const glm::vec2&, const glm::vec2& );
   
   glm::dquat _yawPitchRoll;
   double _distance;
   glm::dquat _rotation;
   glm::dvec3 _center;
    
   std::shared_ptr<SceneObject> tracking;
   std::shared_ptr<SceneObject> tethered;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive& ar, const unsigned int version);
};

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY(IMotionController)
BOOST_CLASS_EXPORT_KEY(Orbit)
