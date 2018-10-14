//
//  Scene.hpp
//  Infinitaruim-Engine
//
//  Created by Trystan Larey-Williams on 10/13/18.
//

#pragma once

#include <string>
#include <vector>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>

#include "Renderable.hpp"

class Scene {
public:
   Scene() {}
   void save( const std::string& ) const;
   void load( const std::string& );
   void add( const std::shared_ptr<IRenderable>& );
   
private:
   std::vector<std::shared_ptr<IRenderable>> renderables;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      ar & renderables;
   }
};
