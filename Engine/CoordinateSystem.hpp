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
   enum Unit {
      Meter,
      Kilometer,
      AstronomicalUnit,
      LightYear,
      Parsec,
      KiloParsec,
      MegaParsec
   };
   
   UniversalPoint convert( Unit );
   
private:
   Unit unit;
   glm::dvec3 coords;
};

class CoordinateSystem : public SceneObject {
public:
   CoordinateSystem() {}
   
   void update( UpdateParams& ) override; // Convert matrix to this coordinate system if position outside radius, traverse proxy
   
private:
   // Will need crossfade between these
   std::shared_ptr<SceneObject> subgraph; // Visible when inside of system at this system's scale
   std::shared_ptr<SceneObject> proxy; // Visible when outside of system at parent system sclae (optional)
   
   UniversalPoint center;
   double radius;
};

