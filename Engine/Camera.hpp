//
//  Camera.hpp
//  IEPlatform
//
//  Created by Trystan (Home) on 2/20/19.
//

#pragma once

#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "RenderPass.hpp"
#include "Renderable.hpp"
#include "MotionControllerOrbit.hpp"
#include "ConsoleInterface.hpp"

class Camera : public Reflection::IConsole<Camera> {
public:
   Camera();
   void Draw();
   
   void add( const std::shared_ptr<IRenderable>& );
   std::shared_ptr<IRenderable> getRenderable( unsigned int indx );
   unsigned int numRenderables();
   
   void setMotionController( std::shared_ptr<IMotionController>& );
   void setRenderContext( std::shared_ptr<IRenderContext>& );
   
   auto reflect() {  // IConsole /////////////////////
      static auto tup = make_tuple(
                                   REFLECT_METHOD(&Camera::add, add),
                                   REFLECT_METHOD(&Camera::getRenderable, getRenderable),
                                   REFLECT_METHOD(&Camera::numRenderables, numRenderables)
                                   );
      
      return tup;
   }
   
private:
   std::shared_ptr<IRenderContext> renderContext;
   std::vector<std::shared_ptr<IRenderPass>> renderPasses;
   
   std::shared_ptr<IMotionController> motionController;
   std::vector<std::shared_ptr<IRenderable>> renderables;
   glm::mat4x4 view;
};


