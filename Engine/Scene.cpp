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
   
   std::wifstream ifs( filename );
   if( !ifs.is_open() ) {
      std::cout<<"Unable to open "<<filename<<". Do you have read permissions?"<<std::endl;
	  return;
   }
   
   // According to boost docs we want the wide version for portable UTF-8
   boost::archive::xml_wiarchive ia( ifs );
   
   {
      std::lock_guard<std::mutex> lock( loadLock );
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
   std::wofstream ofs( filename );
   if( !ofs.is_open() ) {
      std::cout<<"Unable to open "<<filename<<". Do you have write permissions?"<<std::endl;
   }
   
   // According to boost docs we want the wide version for portable UTF-8
   boost::archive::xml_woarchive oa( ofs );
   oa << boost::serialization::make_nvp( "Scene", *this );
   
   ofs.close();
}

void Scene::update() {
   std::lock_guard<std::mutex> lock( loadLock );
   glm::mat4x4 ident;
   
   for( auto& camera : cameras )
      camera->update( ident );
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


