//
//  Camera.cpp
//  IEPlatform
//
//  Created by Trystan (Home) on 2/20/19.
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

void Camera::update( UpdateParams& /* identity */ ) {
   if( motionController )
      motionController->processEvents();
   
   Transform::prepare( *renderContext );
}

void Camera::render( IRenderPass& ) {
   if( dirty ) {
      dirty = false;
      renderPass->prepare( *renderContext );
   }
 
   for (unsigned int i = 0; i < renderContext->getPerspectiveCount(); ++i ) {
       glm::mat4 eye{ 1.0 };
       if (motionController)
           motionController->getViewMatrix(eye);

       glm::mat4x4 proj, view;
       renderContext->getPerspective(i, proj, view);
       if (proj == glm::mat4(1.0))
           proj = projection;

       UpdateParams params(proj, eye * view, *this);
       Transform::update(params);

       renderPass->begin( *renderContext );
       Transform::render( *renderPass );
       renderPass->end( *renderContext );
   }

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
   
   // TODO: This strategy ends up circumventing boost::serialization object tracking. We'll
   // need to do it manually for platform specific instances such as this.
   std::unique_ptr<RenderPassProxy> rp;
   ar >> BOOST_SERIALIZATION_NVP(rp);
   renderPass = IRenderPass::Clone( *rp );
   
   std::unique_ptr<RenderContextProxy> rc;
   ar >> BOOST_SERIALIZATION_NVP(rc);
   renderContext = IRenderContext::Clone( *rc );

   // Subscribe to events from the platform layer that notify of the need to dirty and re-init camera's subgraph (e.g window size or restore).
   auto fun = [this](const IRenderContext& renderContext) {
       if (this->renderContext.get() != &renderContext)
           return;

       std::function<void(SceneObject&)> f{ [](SceneObject& obj) { obj.setDirty(); } };
       visit( Visitor(f) );
   };
   std::shared_ptr<IDelegate> delegate = std::make_shared<EventDelegate<decltype(fun), const IRenderContext&>>(fun);
   IApplication::Create()->subscribe("resetScene", delegate);
}

template<class Archive> void Camera::save( Archive& ar ) const {
   ar << boost::serialization::make_nvp("Transform", boost::serialization::base_object<Transform>(*this));
   
   ar << BOOST_SERIALIZATION_NVP(motionController);
   
   // Force renderPass to be serialized as base class to maintain scene file platform independence.
   std::unique_ptr<RenderPassProxy> rp = std::make_unique<RenderPassProxy>(*renderPass);
   ar << BOOST_SERIALIZATION_NVP(rp);
  
   std::unique_ptr<RenderContextProxy> rc = std::make_unique<RenderContextProxy>(*renderContext);
   ar << BOOST_SERIALIZATION_NVP(rc);
}

namespace boost { namespace serialization {
   template<class Archive> inline void load(Archive& ar, Camera& t, unsigned int version) {
      std::cout<<"Loading Camera"<<std::endl;
      t.load( ar );
   }
   template<class Archive> inline void save(Archive& ar, const Camera& t, const unsigned int version) {
      std::cout<<"Saving Camera"<<std::endl;
      t.save( ar );
   }
}}

template<class Archive> void Camera::serialize(Archive& ar, const unsigned int version) {
	boost::serialization::void_cast_register<Camera, Transform>();
	boost::serialization::split_free(ar, *this, version);
}
