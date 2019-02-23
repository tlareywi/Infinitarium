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
   clear();
   projection = glm::perspective( glm::radians(60.0), 16.0 / 9.0, 0.0001, 100.0 );
}

template<class Archive> void Scene::serialize(Archive & ar, const unsigned int version) {
   std::cout<<"Serializing Scene"<<std::endl;
   ar & renderables;
}

void Scene::clear() {
   {
      std::lock_guard<std::mutex> lock( loadLock );
      renderPass = IRenderPass::Create();
      renderables.clear();
   }
   
   add( std::make_shared<ClearScreen>() );
}

void Scene::load( const std::string& filename ) {
   clear();
   
   std::ifstream ifs( filename, std::ofstream::binary );
   if( !ifs.is_open() ) {
      std::cout<<"Unable to open "<<filename<<". Do you have read permissions?"<<std::endl;
   }
   
   boost::archive::binary_iarchive ia( ifs );
   
   {
      std::lock_guard<std::mutex> lock( loadLock );
      ia >> *this;
   }
   
   ifs.close();
}

void Scene::save( const std::string& filename ) const {
   std::ofstream ofs( filename, std::ofstream::binary );
   if( !ofs.is_open() ) {
      std::cout<<"Unable to open "<<filename<<". Do you have write permissions?"<<std::endl;
   }
   
   boost::archive::binary_oarchive oa( ofs );
   oa << *this;
   
   ofs.close();
}

void Scene::update() {
   glm::mat4 view;
   
   if( motionController ) {
      motionController->processEvents();
      motionController->getViewMatrix( view );
   }
   
   glm::mat4 mvp = projection * view;

   {
      std::lock_guard<std::mutex> lock( loadLock );
      for( auto& renderable : renderables ) {
         renderable->update( mvp );
      }
   }
}

void Scene::draw() {
   std::lock_guard<std::mutex> lock( loadLock );
   
   for( auto& c : cameras )
      c->Draw();
}


