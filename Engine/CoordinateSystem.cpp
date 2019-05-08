//
//  CoordinateSystem.cpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 5/1/19.
//

#include "CoordinateSystem.hpp"

#include <boost/serialization/shared_ptr.hpp>

void CoordinateSystem::update( UpdateParams& params ) {
   // Thinking this should always work as long as it's enforced that the scene is composed of larger coordinate systems (in terms of unit size) always being parents of smaller ones.
   // This seems pretty intuitive as well but just calling it out in case I question myself later :P
   
   // TODO: Support 'proxy' subgraph
   
   if( params.getCamera().getMotionController().getUnit == center.getUnit() ) { // Units are the same, we don't need to do anything special.
      SceneObject::update( params );
      return;
   }
   
   if( center.getUnit() < params.unit /* AND we're within the radius */ ) { // Always set active units on motion controller to most granular system.
      // degtermine if we're within the radius
      UpdateParams paramsCopy( params );
      
      
      // Set new 'active' units and view matix
   }
   else if( center.getUnit() > params.unit ) { // Perform conversion and pass converted view matrix and units to children but do not modify motion controller 'base truth'
      // We're gauranteed to be within the radius in this case
   }
}

namespace boost { namespace serialization {
   template<class Archive> inline void serialize(Archive& ar, UniversalPoint& t, unsigned int version) {
      std::cout<<"Serializing UniversalPoint"<<std::endl;
      
      ar & t.unit;
      ar & t.point;
   }
   
   template<class Archive> inline void serialize(Archive& ar, CoordinateSystem& t, unsigned int version) {
      std::cout<<"Serializing CoordinateSystem"<<std::endl;
      
      ar & t.proxy;
      ar & t.center;
      ar & t.radius;
      
      boost::serialization::void_cast_register<CoordinateSystem,SceneObject>();
      ar & boost::serialization::base_object<SceneObject>(t);
   }
}}
