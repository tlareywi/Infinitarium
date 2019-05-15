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
#include <boost/multiprecision/mpfr.hpp>

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(UniversalPoint)
BOOST_CLASS_EXPORT(CoordinateSystem)

double UniversalPoint::distance( const UniversalPoint& p ) const {
   boost::multiprecision::mpf_float_100 ax{point.x}, ay{point.y}, az{point.z};
   boost::multiprecision::mpf_float_100 bx{p.point.x}, by{p.point.y}, bz{p.point.z};
   
   // TODO: this isn't correct yet. Need to integrate a multiplier as well. Not everyting's
   // a power of 10. 
   boost::multiprecision::mpf_float_100 mult{pow(10.0, p.unit - unit)};
   
   bx *= mult; by *= mult; bz *= mult;
   
   boost::multiprecision::mpf_float_100 d{ boost::multiprecision::sqrt( (ax - bx)*(ax - bx) + (ay - by)*(ay - by) + (az - bz)*(az - bz) ) };
   
   return d.convert_to<double>();
}

UniversalPoint UniversalPoint::convert( UniversalPoint::Unit u ) const {
   boost::multiprecision::mpf_float_100 ax{point.x}, ay{point.y}, az{point.z};
   boost::multiprecision::mpf_float_100 mult{pow(10.0, u - unit)};
   
   UniversalPoint p;
   p.unit = u;
   ax *= mult; ay *= mult; az *= mult;
   p.point.x = ax.convert_to<double>();
   p.point.y = ay.convert_to<double>();
   p.point.z = az.convert_to<double>();
   
   return p;
}

template<class Archive> void UniversalPoint::serialize( Archive& ar, const unsigned int version ) {
   std::cout<<"Serializing UniversalPoint"<<std::endl;
   
   ar & unit;
   ar & point;
}


void CoordinateSystem::update( UpdateParams& params ) {
   // Render pass is not a navigational camera. Nothing to do.
   if( !params.getCamera().getMotionController() ) {
      SceneObject::update( params );
      return;
   }
   
   UniversalPoint currentHome = params.getCamera().getMotionController()->getHome();
   
   glm::vec3 eye = params.getView()[3];
   UniversalPoint camera{ eye.x, eye.y, eye.z, currentHome.getUnit() };
   double distance = camera.distance( center );
   
   // TODO: Support 'proxy' subgraph
   
   if( params.getCamera().getMotionController()->getHome() == center && distance < radius ) {
      // We're in the current (home) coordinate system.
      SceneObject::update( params );
      return;
   }
   else if( params.getCamera().getMotionController()->getHome() == center ) {
      // We exited the coordinate system
      params.getCamera().getMotionController()->popHome();
      return;
   }
   
   if( center.getUnit() < currentHome.getUnit() && distance < radius )
      params.getCamera().getMotionController()->pushHome( center );
   else if( distance < radius ) {
      // Set model matrix to center of coordinate system.
      glm::dmat4 model(1.0), view(params.getView());
      model = glm::translate( model, center.getPoint() );
      
      // Convert eye component of view matrix to coordinate system units.
      UniversalPoint localEye = camera.convert( center.getUnit() );
      view[3] = glm::dvec4( localEye.getPoint(), 1.0 );
      
      UpdateParams paramsCopy( params, view, model );
      SceneObject::update( paramsCopy );
   }
}

template<class Archive> void CoordinateSystem::serialize( Archive& ar, const unsigned int version ) {
   std::cout<<"Serializing CoordinateSystem"<<std::endl;
   
   ar & proxy;
   ar & center;
   ar & radius;
   
   boost::serialization::void_cast_register<CoordinateSystem,SceneObject>();
   ar & boost::serialization::base_object<SceneObject>(*this);
}

