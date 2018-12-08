//
//  Scene.cpp
//  Infinitaruim-Engine
//
//  Created by Trystan Larey-Williams on 10/13/18.
//

#include "Scene.hpp"

#include "PythonBridge.hpp"

#include <fstream>
#include <iostream>

Scene::Scene() {
   motionController = nullptr;
   renderPass = IRenderPass::Create();
   
   projection = glm::perspective( glm::radians(60.0), 16.0 / 9.0, 0.0001, 100.0 );
   
   reflect();
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

void Scene::setMotionController( std::shared_ptr<IMotionController>& ctrl ) {
   motionController = ctrl;
}

void Scene::setRenderContext( std::shared_ptr<IRenderContext>& r ) {
   renderContext = r;
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
   
   renderPass->renderContext = renderContext;
   
   renderPass->begin();
   
   for( auto& renderable : renderables ) {
      renderable->render( *renderPass );
   }
   
   renderPass->end();
}

void Scene::reflect() {
   auto fun = [] (const char * name, auto& property) {
      std::cout << name << " : " << property << "\n";
   };
   
   Scene& s = *this;
   auto tup = make_tuple( REFLECT_MEMBER(s, foo) );
   auto info = ::reflect(s, tup, fun);
   info();
}

auto Scene::getProp() -> decltype(<#expression#>) {
   auto fun = [] (const char * name, auto& property) {
      std::cout << name << " : " << property << "\n";
   };
   
   Scene& s = *this;
   auto tup = make_tuple( REFLECT_MEMBER(s, foo) );
   auto info = ::reflect(s, tup, fun);
}



