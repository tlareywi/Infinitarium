//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
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
   
protected:
   Spheroid() {}

private:
   // TODO: Avoid replication here for duplicate sphere geometry
   std::shared_ptr<DataPack<SpheroidVertex>> geometry;

   static boost::uuids::uuid geometryId;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive &, unsigned int );
};

BOOST_CLASS_EXPORT_KEY(Spheroid);
BOOST_CLASS_EXPORT_KEY(Spheroid::SpheroidVertex);

class SpheroidEmitter : public Spheroid {
public:
    SpheroidEmitter(unsigned int m, unsigned int p, float o, bool f = false) :
        Spheroid(m, p, o, f) {}

    void update(UpdateParams&) override;

private:
    SpheroidEmitter() {}

    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive&, unsigned int);
};

BOOST_CLASS_EXPORT_KEY(SpheroidEmitter);