//
//  Scene.cpp
//  Infinitaruim-Engine
//
//  Created by Trystan Larey-Williams on 10/13/18.
//

#include "Scene.hpp"

#include <fstream>
#include <iostream>

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(Scene);

Scene::Scene() {
}

void Scene::clear() {
   std::lock_guard<std::mutex> lock( loadLock );
   for( auto& camera : cameras )
      camera = nullptr;
   
   cameras.clear();
}

void Scene::add( const std::shared_ptr<Camera>& camera ) {
   std::lock_guard<std::mutex> lock( loadLock );
   cameras.push_back( camera );
}

void Scene::load( const std::string& filename ) {
   clear();
   
   std::ifstream ifs( filename, std::fstream::binary );
   if( !ifs.is_open() ) {
      std::cout<<"Unable to open "<<filename<<". Do you have read permissions?"<<std::endl;
	  return;
   }
   
   {
      std::lock_guard<std::mutex> lock( loadLock );
      
      // According to boost docs we want the wide version for portable UTF-8
  //    boost::archive::xml_wiarchive ia( ifs );
      boost::archive::binary_iarchive ia( ifs );
      ia >> boost::serialization::make_nvp( "Scene", *this );
      
      for( auto& camera : cameras )
         camera->init();
   }
   
   ifs.close();
}

void Scene::loadLocal( const std::string& sceneName ) {
   load( localScenePath + sceneName );
}

void Scene::setLocalScenePath( const std::string& path ) {
   localScenePath = path;
}

void Scene::save( const std::string& filename ) const {
   std::ofstream ofs( filename, std::fstream::binary );
   if( !ofs.is_open() ) {
      std::cout<<"Unable to open "<<filename<<". Do you have write permissions?"<<std::endl;
   }
   
   {  // According to boost docs we want the wide version for portable UTF-8
   //   boost::archive::xml_woarchive oa( ofs );
      boost::archive::binary_oarchive oa( ofs );
      oa << boost::serialization::make_nvp( "Scene", *this );
   }
   
   ofs.close();
}

void Scene::update() {
   std::lock_guard<std::mutex> lock( loadLock );
   
   for( auto& camera : cameras ) {
      UpdateParams ident( *camera );
      camera->update( ident );
   }
}

void Scene::render() {
   std::lock_guard<std::mutex> lock( loadLock );
   RenderPassProxy stub;
   
   for( auto& camera : cameras )
      camera->render( stub );
}

template<class Archive> void Scene::serialize(Archive & ar, const unsigned int version) {
   std::cout<<"Serializing Scene"<<std::endl;
   ar & BOOST_SERIALIZATION_NVP(cameras);
}


