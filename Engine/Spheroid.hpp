//
//  Spheroid.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 4/17/19.
//

#pragma once

#include "Renderable.hpp"

#include <map>
#include <memory>

class Spheroid : public IRenderable {
public:
   Spheroid( unsigned int, unsigned int, float, bool = false );
   
   void prepare( IRenderContext& ) override;
   
private:
   struct SpheroidVertex {
      glm::fvec3 vertex;
      glm::fvec3 normal;
      glm::fvec2 textCoord;
   };
   
   // TODO: Avoid replication here (both of these) for duplicate sphere geometry
   std::shared_ptr<DataPack<SpheroidVertex>> geometry;
   std::shared_ptr<IDataBuffer> spheroid;
};
