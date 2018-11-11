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

#include <glm/gtc/matrix_transform.hpp>

#include "Renderable.hpp"
#include "RenderPass.hpp"
#include "MotionControllerOrbit.hpp"
#include "RenderContext.hpp"

class Scene {
public:
   Scene();
   void save( const std::string& ) const;
   void load( const std::string& );
   void add( const std::shared_ptr<IRenderable>& );
   
   void setMotionController( std::shared_ptr<IMotionController>& ctrl ) {
      motionController = ctrl;
   }
   
   void setRenderContext( std::shared_ptr<IRenderContext>& r ) {
      renderContext = r;
   }
   
   void update();
   void draw();
   
private:
   std::shared_ptr<IRenderContext> renderContext;
   std::shared_ptr<IMotionController> motionController;
   std::shared_ptr<IRenderPass> renderPass;
   std::vector<std::shared_ptr<IRenderable>> renderables;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      std::cout<<"Serializing Scene"<<std::endl;
      ar & renderables;
   }

   glm::mat4 projection;
};
