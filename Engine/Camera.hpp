//
//  Camera.hpp
//  IEPlatform
//
//  Created by Trystan (Home) on 2/20/19.
//

#pragma once

#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "RenderPass.hpp"
#include "Renderable.hpp"
#include "MotionControllerOrbit.hpp"
#include "ConsoleInterface.hpp"

///
/// \brief A Camera encapsulates a motion controller (which holds the view matrix), a set of renderables, a render pass, the render context (physical device).
/// TOOD: Should contain a viewport was well?
///
class Camera : public Reflection::IConsole<Camera> {
public:
   Camera();
   
   void update();
   void draw();
   
   void addRenderable( const std::shared_ptr<IRenderable>& );
   void setRenderPass( const std::shared_ptr<IRenderPass>& );
   
   std::shared_ptr<IRenderable> getRenderable( unsigned int indx );
   unsigned int numRenderables();
   
   void setMotionController( std::shared_ptr<IMotionController>& );
   void setRenderContext( std::shared_ptr<IRenderContext>& );
   
   auto reflect() {  // IConsole /////////////////////
      static auto tup = make_tuple(
                                   REFLECT_METHOD(&Camera::addRenderable, addRenderable),
                                   REFLECT_METHOD(&Camera::setRenderPass, setRenderPass),
                                   REFLECT_METHOD(&Camera::getRenderable, getRenderable),
                                   REFLECT_METHOD(&Camera::numRenderables, numRenderables)
                                   );
      
      return tup;
   }
   
private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      std::cout<<"Serializing Camera"<<std::endl;
      ar & renderables;
      std::shared_ptr<IRenderPass> rp = std::make_shared<IRenderPass>(*renderPass);
      ar & rp;
   }
   
   bool dirty;
   
   std::shared_ptr<IRenderContext> renderContext;
   // We chould have a pipline class containing multiple renderpasses here but may make more sense to have a Pipline class contain multiple Cameras instead depending on
   // where we stick Viewport.
   std::shared_ptr<IRenderPass> renderPass;
   
   std::shared_ptr<IMotionController> motionController;
   
   // TODO: Open question, if multiple cameras are pointing at the same renderable, is boost serialization smart enough to not load duplicate instances.
   std::vector<std::shared_ptr<IRenderable>> renderables;
   
   glm::mat4 projection;
};


