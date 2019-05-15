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
      Meter = 1,
      Kilometer = 3,
      Megameter = 6,
      AstronomicalUnit = 11,
      Parsec = 16,
      KiloParsec = 19,
      MegaParsec = 22
   };
   
   UniversalPoint() {}
   UniversalPoint( double x, double y, double z, Unit u ) : point(glm::dvec3(x,y,z)), unit(u) {}
   UniversalPoint( const UniversalPoint& p ) : point(p.point), unit(p.unit) {}
   
   bool operator ==( const UniversalPoint& p ) const {
      if( p.unit == unit && p.point == point )
         return true;
      else
         return false;
   }
  
   Unit getUnit() const { return unit; }
   glm::dvec3 getPoint() { return point; }
   
   double distance( const UniversalPoint& ) const;
   UniversalPoint convert( Unit ) const;
   
private:
   glm::dvec3 point;
   Unit unit;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive& ar, const unsigned int );
};

class CoordinateSystem : public SceneObject {
public:
   CoordinateSystem() {}
   CoordinateSystem( const UniversalPoint& c, double r ) : center(c), radius(r) {}
   
   void update( UpdateParams& ) override; // Convert matrix to this coordinate system if position outside radius, traverse proxy
   
private:
   // Will need crossfade between these:
   // Implicit subgraph, SceneObject children, Visible when inside of system at this system's scale
   std::shared_ptr<SceneObject> proxy; // Visible when outside of system at parent system sclae (optional)
   
   UniversalPoint center;
   double radius;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive& ar, const unsigned int );
};

