//
//  CoordinateSystem.cpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 5/1/19.
//

#include "CoordinateSystem.hpp"
#include "Camera.hpp"
#include "UniformType.hpp"

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


void CoordinateSystem::update( UpdateParams& params ) {
   // Render pass is not a navigational camera. Nothing to do.
   if( !params.getCamera().getMotionController() ) {
      SceneObject::update( params );
      return;
   }
   
   // Apply model transform to coordinate system center.
   // TODO: Center needs to be a UniversalPoint in parent units
   glm::dvec3 c = params.getModel() * glm::dvec4(center.getPoint(), 1.0);
   UniversalPoint transformed_center( c, center.getUnit() );
   
   UniversalPoint currentHome = params.getCamera().getMotionController()->getHome();
   
   glm::vec3 eye = params.getView()[3];
   UniversalPoint camera{ eye.x, eye.y, eye.z, currentHome.getUnit() };
   double distance = transformed_center.distance( camera );
   
   // TODO: Support 'proxy' subgraph (have debug geometry always under this?)
   
   if( params.getCamera().getMotionController()->getHome() == center && distance < radius ) {
      // We're in the current (home) coordinate system.
      SceneObject::update( params );
      return;
   }
   else if( params.getCamera().getMotionController()->getHome() == center ) {
      // We exited the coordinate system.
      params.getCamera().getMotionController()->popHome();
      return;
   }
   
   if( center.getUnit() < currentHome.getUnit() && distance < radius ) {
      UniversalPoint newHome( center.getPoint(), units );
      params.getCamera().getMotionController()->pushHome( newHome );
   }
   else if( distance < radius ) { // This is a parent of the current coordinate system
      // Reset model matrix to identity
      glm::dmat4 model(1.0), view(params.getView());
      
      // Convert eye component of view matrix to coordinate system units.
      UniversalPoint localEye = camera.convert( units );
      view[3] = glm::dvec4( localEye.getPoint(), 1.0 );
      
      UpdateParams paramsCopy( params, params.getProjection(), view, model );
      SceneObject::update( paramsCopy );
   }
}

glm::vec3 CoordinateSystem::getCenter() {
   glm::dvec3 c { glm::dvec4(center.getPoint(), 1.0) };
   return c;
}

template<class Archive> void CoordinateSystem::serialize( Archive& ar, const unsigned int version ) {
   std::cout<<"Serializing CoordinateSystem"<<std::endl;
   
   ar & BOOST_SERIALIZATION_NVP(proxy);
   ar & BOOST_SERIALIZATION_NVP(center);
   ar & BOOST_SERIALIZATION_NVP(radius);
   
   boost::serialization::void_cast_register<CoordinateSystem,SceneObject>();
   ar & boost::serialization::make_nvp("SceneObject", boost::serialization::base_object<SceneObject>(*this));
}

