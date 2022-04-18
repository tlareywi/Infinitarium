//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "CoordinateSystem.hpp"
#include "Camera.hpp"
#include "UniformType.hpp"
#include "Spheroid.hpp"

#include <limits>

#include <boost/serialization/shared_ptr.hpp>

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_IMPLEMENT(UniversalPoint)
BOOST_CLASS_EXPORT_IMPLEMENT(CoordinateSystem)

using namespace boost::multiprecision;

double UniversalPoint::distance( const UniversalPoint& p ) const {
   mpf ax{point.x}, ay{point.y}, az{point.z};
   mpf bx{p.point.x}, by{p.point.y}, bz{p.point.z};
   
   mpf mult{ getMultiplier(unit, p.unit) };
   
   bx *= mult; by *= mult; bz *= mult;
   
   mpf d{ boost::multiprecision::sqrt( (ax - bx)*(ax - bx) + (ay - by)*(ay - by) + (az - bz)*(az - bz) ) };
   
   // In p's units
   return d.convert_to<double>();
}

UniversalPoint UniversalPoint::convert( UniversalPoint::Unit u ) const {
   mpf ax{point.x}, ay{point.y}, az{point.z};
   mpf mult{ getMultiplier(unit, u) };
   
   UniversalPoint p;
   p.unit = u;
   ax *= mult; ay *= mult; az *= mult;
   p.point.x = ax.convert_to<double>();
   p.point.y = ay.convert_to<double>();
   p.point.z = az.convert_to<double>();
   
   return p;
}

double UniversalPoint::convert( double d , Unit targetUnits ) const {
    mpf ad{ d };
    mpf mult{ getMultiplier(unit, targetUnits) };

    ad *= mult;
    return ad.convert_to<double>();
}

mpf UniversalPoint::toMeters( Unit source ) const {
	mpf retVal{ 1.0 };
    
    switch( source ) {
       case Kilometer:
          retVal = 1000.0;
          break;
       case Megameter:
          retVal = 1000000.0;
          break;
       case AstronomicalUnit:
          retVal = 149597870700.0;
          break;
       case LightYear:
          retVal = 9460730472580800.0;
          break;
       case Parsec:
          retVal = 3.085677581e16;
          break;
       case KiloParsec:
          retVal = 3.085677581e19;
          break;
       case MegaParsec:
          retVal = 3.085677581e22;
          break;
       case Meter:
       default:
          break;
    }
    
    return retVal;
}

mpf UniversalPoint::getMultiplier( Unit source, Unit dest ) const {
   return toMeters( source ) / toMeters( dest );
}

template<class Archive> void UniversalPoint::serialize( Archive& ar, const unsigned int version ) {
   std::cout<<"Serializing UniversalPoint"<<std::endl;
   
   ar & BOOST_SERIALIZATION_NVP(unit);
   ar & BOOST_SERIALIZATION_NVP(point);
}

/// <summary>
/// CoordinateSystem
/// </summary>
/// <param name="params"></param>

CoordinateSystem::CoordinateSystem(const UniversalPoint& c, double r, UniversalPoint::Unit u) : 
    center(c), radius(r), units(u) {
    
    // Debug geometry, wireframe sphere of radius r
    auto sphere = std::make_shared<Spheroid>(40, 40, 0.0f, false);
    sphere->setPolygonMode(IRenderState::PolygonMode::Line);
    sphere->setCullMode(IRenderState::CullMode::None);
    sphere->setProgram("default");

    UniversalPoint p{ 0.0, 0.0, 0.0, u};
    double radiusParentUnits = p.convert(r, c.getUnit());

    // Scale unit sphere buy radius
    auto scale = std::make_shared<Transform>();
    scale->scale((float)radiusParentUnits, (float)radiusParentUnits, (float)radiusParentUnits);
    scale->applyTranslate(c.getPoint());
    proxy = scale;

    proxy->addChild(sphere);
}

void CoordinateSystem::prepare(IRenderContext& ctx) {
    proxy->prepare(ctx);
    SceneObject::prepare(ctx);
}

void CoordinateSystem::update( UpdateParams& params ) {
   if (dirty) {
      dirty = false;
      prepare( *(params.getCamera().getContext()) );
   }

   if (!active) {
       proxy->update(params);
   }

   auto motionController = params.getCamera().getMotionController();

   // Render pass is not a navigational camera. Nothing to do.
   if( !motionController ) {
      SceneObject::update( params );
      return;
   }

   static double restoreMultiple = 1.0;

   // Nested Coordinate systems can stomp on each others view matrix. Start with one direct from
   // the motion controller unless overriden later.
   glm::dmat4 view;
   motionController->getViewMatrix(view);
   UpdateParams origView(params, params.getProjection(), view, params.getModel());
   
   // Apply model transform to coordinate system center.
   glm::dvec3 c = glm::dvec4(center.getPoint(), 1.0); // Parent units
   UniversalPoint transformed_center( c, center.getUnit() ); // Parent units
   UniversalPoint currentHome = motionController->getHome(); // Units of 'active' CoordinateSystem
   UniversalPoint newHome(center.getPoint(), units);
   
   glm::vec3 eye = glm::inverse(view)[3];
   UniversalPoint camera{ eye.x, eye.y, eye.z, currentHome.getUnit() };
   double distance = transformed_center.distance( camera ); // distance in parent CoordinateSystem units
   double radiusInActiveUnits = newHome.convert(radius, currentHome.getUnit());

   // Since an active coordinate system always shifts origin to 0,0,0, the distance from the center is 
   // just the length of the eye vector.
   double distanceActive = glm::length(eye); // distance in active CoordinateSystem units

   //std::cout << "radiusInActiveUnits " << radiusInActiveUnits << " distance " << distance << " units " << units << std::endl;
   //std::cout << " distanceActive " << distanceActive << std::endl;
  
   if( motionController->getHome() == newHome && distanceActive < radius ) {
      // Active system is in the same units as this system 'and' distance from center is less than radius;
      // radius being in this system's units. Traverse this CoordinateSystem's sub-graph.
      active = true;
      SceneObject::update(origView);
   }
   else if ( motionController->getHome() == newHome /* distanceActive > radius implied */ ) {
      // Active system is in the same units as this system but we've exited. Pop to parent units.
      // Do not traverse sub-graph.
      std::cout << "Deactiveate Subscene" << std::endl;
      active = false;
      motionController->setMovementMultiple(restoreMultiple);
      motionController->popHome();
   }
   else if( units < currentHome.getUnit() && distance < radiusInActiveUnits) {
      // Active system is a parent of the current system and we've crossed the boundry into this system.
      // Set as new active system. 
      std::cout << "Activate Subscene" << std::endl;
      active = true;
      motionController->pushHome( newHome );
      restoreMultiple = motionController->setMovementMultiple( radius / 1000.0 );
   }
   else if( active ) {
      // A nested system is active below us, but still want to render sub-graph of this system.
      // Obtain a view matrix thats scaled for this coordinate system and override the active one.
      glm::dmat4 localView = motionController->localView( motionController->childSystem(newHome) );
      
      UpdateParams paramsCopy( params, params.getProjection(), localView, params.getModel() );
      SceneObject::update( paramsCopy );
   }
}

void CoordinateSystem::render(IRenderPass& renderPass) {
    if (!active) {
        proxy->render(renderPass);
    }
    else {
        SceneObject::render(renderPass);
    }
}

glm::vec3 CoordinateSystem::getCenter() {
   glm::dvec3 c { glm::dvec4(center.getPoint(), 1.0) };
   return c;
}

template<class Archive> void CoordinateSystem::serialize( Archive& ar, const unsigned int version ) {
   std::cout<<"Serializing CoordinateSystem"<<std::endl;
   
   ar & proxy;
   ar & center;
   ar & radius;
   ar & units;
   
   boost::serialization::void_cast_register<CoordinateSystem,SceneObject>();
   ar & boost::serialization::make_nvp("SceneObject", boost::serialization::base_object<SceneObject>(*this));
}

