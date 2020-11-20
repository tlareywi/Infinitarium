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
#include "Transform.hpp"
#include "MotionControllerOrbit.hpp"
#include "ConsoleInterface.hpp"

#include "../config.h"

///
/// \brief A Camera encapsulates a motion controller (which holds the view matrix), a set of renderables, a render pass, the render context (physical device).
/// TOOD: Should contain a viewport was well?
///
class Camera : public Transform, public Reflection::IConsole<Camera> {
public:
   Camera();
   virtual ~Camera() {}
   
   void init();
   
   void update( UpdateParams& ) override;
   void render( IRenderPass& ) override;
   
   void setRenderPass( const std::shared_ptr<IRenderPass>& );
   
   void setMotionController( const std::shared_ptr<IMotionController>& );
   std::shared_ptr<IMotionController> getMotionController();
   void setRenderContext( const std::shared_ptr<IRenderContext>& );
   
   auto reflect() {  // IConsole /////////////////////
      static auto tup = make_tuple(
                                   REFLECT_METHOD(&Camera::setRenderPass, setRenderPass),
                                   REFLECT_METHOD(&Camera::setRenderContext, setRenderContext)
                                   );
      
      return tup;
   }
   
   template<class Archive> void save( Archive& ) const;
   template<class Archive> void load( Archive& );
   
private:
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive&, const unsigned int);
   
   std::shared_ptr<IRenderContext> renderContext;
   // We chould have a pipline class containing multiple renderpasses here but may make more sense to have a Pipline class contain multiple Cameras instead depending on
   // where we stick Viewport.
   std::shared_ptr<IRenderPass> renderPass;
   
   std::shared_ptr<IMotionController> motionController;
   
   glm::mat4 projection;
};

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY(Camera);


