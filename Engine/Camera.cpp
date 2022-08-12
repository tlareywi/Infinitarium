//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "Camera.hpp"
#include "Delegate.hpp"
#include "Application.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Camera)

Camera::Camera() : motionController(nullptr) {
   projection = glm::infinitePerspective( glm::radians(60.0), 16.0 / 9.0, 0.0001 );
   renderPass = nullptr;
   renderContext = nullptr;
}

void Camera::setMotionController( const std::shared_ptr<IMotionController>& ctrl ) {
   motionController = ctrl;
}

std::shared_ptr<IMotionController> Camera::getMotionController() {
   return motionController;
}

void Camera::setRenderContext( const std::shared_ptr<IRenderContext>& r ) {
   renderContext = r;
   dirty = true;
}

void Camera::setRenderPass( const std::shared_ptr<IRenderPass>& rp ) {
   renderPass = rp;
}

std::shared_ptr<IRenderContext> Camera::getContext() {
   return renderContext;
}

void Camera::update( UpdateParams& params ) {
   if( dirty )
       Transform::prepare( *renderContext );

   glm::dmat4 view{ 1.0 };
   if (motionController) {
       motionController->processEvents( params );

	   static double fov{ 60.0 };
	   motionController->getViewMatrix(view);
	   if (motionController->getFOV() != fov) {
		   fov = motionController->getFOV();
		   projection = glm::infinitePerspective(glm::radians(fov), 16.0 / 9.0, 0.0001);
	   }
   }

   UpdateParams updateParams(params, projection, view, glm::dmat4(1.0));
   Transform::update(updateParams);
    
   if( motionController )
       motionController->postUpdate();

   //updateParams = std::make_unique<UpdateParams>(params);
}

void Camera::render( IRenderPass& ) {
   if( dirty ) {
      dirty = false;
      renderPass->prepare( *renderContext );
   }
 
   //
   // This was for VR integration but broke other things. Find another way that doesn't require
   // traversing the update path multiple times. That makes no sense.
   //
   /* for (unsigned int i = 0; i < renderContext->getPerspectiveCount(); ++i) {
       glm::dmat4 eye{ 1.0 };
	   if (motionController) {
		   static double fov{ 60.0 };
		   motionController->getViewMatrix(eye);
		   if (motionController->getFOV() != fov) {
			   fov = motionController->getFOV();
			   projection = glm::infinitePerspective(glm::radians(fov), 16.0 / 9.0, 0.0001);
		   }
	   }

       glm::dmat4 proj, view;
       renderContext->getPerspective(i, proj, view);
       if (proj == glm::dmat4(1.0))
           proj = projection; // Non-VR case

       UpdateParams params(*updateParams, proj, eye * view, glm::dmat4(1.0));
       Transform::update(params);

       renderPass->begin( *renderContext );
       Transform::render( *renderPass );
       renderPass->end( *renderContext );
   } */

   renderPass->begin(*renderContext);
   Transform::render(*renderPass);
   renderPass->end(*renderContext);

   renderPass->runPostRenderOperations();
}

//////////////////////////////////////////////////////////////////////////////////////////
// SERIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////
template<class Archive> void Camera::load( Archive& ar ) {
	ar >> boost::serialization::make_nvp("Transform", boost::serialization::base_object<Transform>(*this));

	ar >> BOOST_SERIALIZATION_NVP(motionController);

	if( motionController.get() ) {
		// The assumption here is if we set a motion controller then we're the main navigational eye/camera.
	    // Make responder to runtime add node events. Should refactor at some point.
	    auto fun = [this]( const std::shared_ptr<SceneObject>& n ) {
	       addChild( n );
	    };
	    std::shared_ptr<IDelegate> delegate = std::make_shared<EventDelegate<decltype(fun), const std::shared_ptr<SceneObject>&>>( fun );
	    IApplication::Create()->subscribe("addSubgraph", delegate);
	 }

	 RenderPassProxy rp;
	 ar >> rp;
	 renderPass = IRenderPass::Clone( rp );

	 RenderContextProxy rc;
	 ar >> rc;
	 renderContext = IRenderContext::Clone( rc );

	 // Subscribe to events from the platform layer that notify of the need to dirty and re-init camera's subgraph (e.g window size or restore).
	 auto fun = [this](const IRenderContext& renderContext) {
	     if (this->renderContext.get() != &renderContext)
	         return;

	     std::function<bool(SceneObject&)> f{ [](SceneObject& obj) {
	         obj.setDirty();
	         return true;
	     }};
	     visit( Visitor(f) );
	 };
	 std::shared_ptr<IDelegate> delegate = std::make_shared<EventDelegate<decltype(fun), const IRenderContext&>>(fun);
	 IApplication::Create()->subscribe("resetScene", delegate);
}

template<class Archive> void Camera::save( Archive& ar ) const {
	 if (!renderPass)
	    throw std::runtime_error("Fatal: Attempted to serialize Camera with no RenderPass");
	 if (!renderContext)
	    throw std::runtime_error("Fatal: Attempted to serialize Camera with no RenderContext");

	 ar << boost::serialization::make_nvp("Transform", boost::serialization::base_object<Transform>(*this));

	 ar << BOOST_SERIALIZATION_NVP(motionController);

	 // Force renderPass to be serialized as base class to maintain scene file platform independence.

     // Memory leak but we don't care. This will ensure we get a unique address for each proxy object so
     // boost doesn't try to skip serializing the object. If we create the proxy on the stack, it's the 
     // same address every time (at at least often) so boost thinks it's the same object.
	 ar << *(new RenderPassProxy(*renderPass));
	 ar << *(new RenderContextProxy(*renderContext));
}

namespace boost { namespace serialization {
   template<class Archive> inline void load(Archive& ar, Camera& t, unsigned int version) {
      t.load( ar );
	  std::cout << "Loading Camera" << t.getName() << std::endl;
   }
   template<class Archive> inline void save(Archive& ar, const Camera& t, const unsigned int version) {
      std::cout<<"Saving Camera "<< t.getName() << std::endl;
      t.save( ar );
	  std::cout << "=================================================================================" << std::endl;
   }
}}

template<class Archive> void Camera::serialize(Archive& ar, const unsigned int version) {
	boost::serialization::void_cast_register<Camera, Transform>();
	boost::serialization::split_free(ar, *this, version);
}
