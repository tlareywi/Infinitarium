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

#include <glm/gtc/matrix_transform.hpp>

#include "Renderable.hpp"
#include "RenderPass.hpp"
#include "MotionControllerOrbit.hpp"
#include "RenderContext.hpp"
#include "ConsoleInterface.hpp"

class Scene : public Reflection::IConsole<Scene> {
public:
   Scene();
   virtual ~Scene() {}
   void save( const std::string& ) const;
   void load( const std::string& );
   
   void add( const std::shared_ptr<IRenderable>& );
   std::shared_ptr<IRenderable> getRenderable( unsigned int indx );
   unsigned int numRenderables();
   
   void setMotionController( std::shared_ptr<IMotionController>& );
   void setRenderContext( std::shared_ptr<IRenderContext>& );
   
   void update();
   void draw();
   
   auto reflect() {  // IConsole /////////////////////
      static auto tup = make_tuple(
         REFLECT_METHOD(&Scene::save, save),
         REFLECT_METHOD(&Scene::load, load),
         REFLECT_METHOD(&Scene::add, add),
         REFLECT_METHOD(&Scene::getRenderable, getRenderable),
         REFLECT_METHOD(&Scene::numRenderables, numRenderables)
      );
      
      return tup;
   }
   
private:
   std::shared_ptr<IRenderContext> renderContext;
   std::shared_ptr<IMotionController> motionController;
   std::shared_ptr<IRenderPass> renderPass;
   std::vector<std::shared_ptr<IRenderable>> renderables;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version);

   glm::mat4 projection;
};
