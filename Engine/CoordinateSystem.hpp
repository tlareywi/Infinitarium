//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#define GLM_FORCE_RADIANS
#define GLM_DEPTH_ZERO_TO_ONE

#include <glm/gtc/matrix_transform.hpp>

#include "SceneObject.hpp"

#pragma warning( push )
#pragma warning( disable : 4244 ) // The underlying gmp header generates lots of spurious type conversion warnings.
#pragma warning( push )
#pragma warning( disable : 4146 )
#if defined(WIN32)
#include <boost/multiprecision/cpp_bin_float.hpp>
using mpf = boost::multiprecision::cpp_bin_float_100;
#else
#include <boost/multiprecision/mpfr.hpp> // Faster support lib for OSX and Linux
using mpf = boost::multiprecision::mpf_float_100;
#endif
#pragma warning( pop )
#pragma warning( pop )

class UniversalPoint {
public:
   enum Unit {
      Meter,
      Kilometer,
      Megameter,
      AstronomicalUnit,
      LightYear,
      Parsec,
      KiloParsec,
      MegaParsec
   };
   
   UniversalPoint() {}
   UniversalPoint( double x, double y, double z, Unit u ) : point(glm::dvec3(x,y,z)), unit(u) {}
   UniversalPoint( const UniversalPoint& p ) : point(p.point), unit(p.unit) {}
   UniversalPoint( const glm::dvec3& v, Unit u ) : point(v), unit(u) {}
   
   bool operator ==( const UniversalPoint& p ) const {
      if( p.unit == unit && p.point == point )
         return true;
      else
         return false;
   }
   
   mpf getMultiplier(Unit, Unit) const;
  
   Unit getUnit() const { return unit; }
   glm::dvec3 getPoint() { return point; }
   
   double distance( const UniversalPoint& ) const;
   UniversalPoint convert( Unit ) const;
   
private:
   mpf toMeters(Unit) const;
   
   glm::dvec3 point;
   Unit unit;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive& ar, const unsigned int );
};


class CoordinateSystem : public SceneObject {
public:
   CoordinateSystem() {}
   CoordinateSystem( const UniversalPoint& c, double r, UniversalPoint::Unit u ) : center(c), radius(r), units(u) {}
   
   void update( UpdateParams& ) override; // Convert matrix to this coordinate system if position outside radius, traverse proxy
   
   // TODO: this assumes the viewer is outside the system. May not always return value in expected units.
   glm::vec3 getCenter() override;
   
private:
   // Will need crossfade between these:
   // Implicit subgraph, SceneObject children, Visible when inside of system at this system's scale
   std::shared_ptr<SceneObject> proxy; // Visible when outside of system at parent system sclae (optional)
   
   UniversalPoint center; // Center in parent units
   double radius; // In the units below
   UniversalPoint::Unit units; // Units of this system
   
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive& ar, const unsigned int );
};

