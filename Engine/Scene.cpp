//
//  Scene.cpp
//  Infinitaruim-Engine
//
//  Created by Trystan Larey-Williams on 10/13/18.
//

#include "Scene.hpp"

#include <fstream>

void Scene::load( const std::string& filename ) {
   std::ifstream ifs( filename );
   boost::archive::binary_iarchive ia( ifs );
   ia >> *this;
}

void Scene::save( const std::string& filename ) const {
   std::ofstream ofs( filename );
   boost::archive::binary_oarchive oa( ofs );
   oa << *this;
}

void Scene::add( const std::shared_ptr<IRenderable>& renderable ) {
   renderables.push_back( renderable );
}


