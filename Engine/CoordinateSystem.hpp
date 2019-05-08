//
//  CoordinateSystem.hpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 5/1/19.
//

#pragma once

#include <glm/glm.hpp>
#include "SceneObject.hpp"

class UniversalPoint {
public:
   enum Unit {
      Meter,
      Kilometer,
      AstronomicalUnit,
      LightYear,
      Parsec,
      KiloParsec,
      MegaParsec
   };
   
   UniversalPoint( double x, double y, double z, Unit u ) : point(glm::dvec3(x,y,z)), unit(u) {}
   Unit getUnit() { return unit; }
   
//   UniversalPoint convert( Unit );
   
private:
   glm::dvec3 point;
   Unit unit;
   
   friend class boost::serialization::access;
   template<class Archive> friend void boost::serialization::serialize( Archive&, UniversalPoint&, unsigned int );
};

class CoordinateSystem : public SceneObject {
public:
   CoordinateSystem( const UniversalPoint& c, double r ) : center(c), radius(r) {}
   
   void update( UpdateParams& ) override; // Convert matrix to this coordinate system if position outside radius, traverse proxy
   
private:
   // Will need crossfade between these:
   // Implicit subgraph, SceneObject children, Visible when inside of system at this system's scale
   std::shared_ptr<SceneObject> proxy; // Visible when outside of system at parent system sclae (optional)
   
   UniversalPoint center;
   double radius;
   
   friend class boost::serialization::access;
   template<class Archive> friend void boost::serialization::serialize( Archive&, CoordinateSystem&, unsigned int );
};

