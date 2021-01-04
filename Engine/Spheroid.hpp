//
//  Spheroid.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 4/17/19.
//

#pragma once

#include "Renderable.hpp"
#include "ObjectStore.hpp"

class Spheroid : public IRenderable {
public:
   Spheroid( unsigned int, unsigned int, float, bool = false );
   
   void prepare( IRenderContext& ) override;
   
   struct SpheroidVertex {
       friend class boost::serialization::access;

       alignas(16) glm::fvec3 vertex;
       alignas(16) glm::fvec3 normal;
       alignas(8)  glm::fvec2 texCoord;
       
       template<class Archive> void serialize( Archive &, unsigned int );
   };
   
private:
   Spheroid() {}
   
   // TODO: Avoid replication here for duplicate sphere geometry
   std::shared_ptr<DataPack<SpheroidVertex>> geometry;

   static boost::uuids::uuid geometryId;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive &, unsigned int );
};

BOOST_CLASS_EXPORT_KEY(Spheroid);
BOOST_CLASS_EXPORT_KEY(Spheroid::SpheroidVertex);