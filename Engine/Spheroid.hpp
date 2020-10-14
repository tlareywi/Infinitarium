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
   
   struct SpheroidVertex {
      glm::fvec3 vertex;
      glm::fvec3 normal;
      glm::fvec2 textCoord;
      friend class boost::serialization::access;
      template<class Archive> void serialize( Archive &, unsigned int );
   };
   
private:
   Spheroid() {}
   
   // TODO: Avoid replication here (both of these) for duplicate sphere geometry
   std::shared_ptr<DataPack<SpheroidVertex>> geometry;
   std::shared_ptr<IDataBuffer> spheroid;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive &, unsigned int );
};
