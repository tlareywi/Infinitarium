//
//  Scene.cpp
//  Infinitaruim-Engine
//
//  Created by Trystan Larey-Williams on 10/13/18.
//

#include "Scene.hpp"

#include <fstream>
#include <iostream>

Scene::Scene() {
   motionController = nullptr;
   renderPass = IRenderPass::Create();
   
   projection = glm::perspective( glm::radians(60.0), 16.0 / 9.0, 0.0001, 100.0 );
}

void Scene::load( const std::string& filename ) {
   std::ifstream ifs( filename, std::ofstream::binary );
   if( !ifs.is_open() ) {
      std::cout<<"Unable to open "<<filename<<". Do you have read permissions?"<<std::endl;
   }
   boost::archive::binary_iarchive ia( ifs );
   ia >> *this;
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

void Scene::add( const std::shared_ptr<IRenderable>& renderable ) {
   renderables.push_back( renderable );
}

void Scene::update() {
   glm::mat4 view;
   
   if( motionController ) {
      motionController->processEvents();
      motionController->getViewMatrix( view );
   }
   
   glm::mat4 mvp = projection * view;

   for( auto& renderable : renderables ) {
      renderable->update( mvp );
   }
}

void Scene::draw() {
   
   renderPass->begin();
   
   for( auto& renderable : renderables ) {
      renderable->render( *renderPass );
   }
   
   renderPass->end();
}


