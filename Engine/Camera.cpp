//
//  Camera.cpp
//  IEPlatform
//
//  Created by Trystan (Home) on 2/20/19.
//

#include "Camera.hpp"

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_IMPLEMENT(Camera)

Camera::Camera() : dirty(false), motionController(nullptr), name("clear") {
   addRenderable( std::make_shared<ClearScreen>() );
   projection = glm::perspective( glm::radians(60.0), 16.0 / 9.0, 0.0001, 100.0 );
   renderPass = IRenderPass::Create();
   renderContext = IRenderContext::Create(0, 0, 1, 1, false);
}

void Camera::setMotionController( const std::shared_ptr<IMotionController>& ctrl ) {
   motionController = ctrl;
}

void Camera::setRenderContext( const std::shared_ptr<IRenderContext>& r ) {
   renderContext = r;
   dirty = true;
}

void Camera::addRenderable( const std::shared_ptr<IRenderable>& renderable ) {
   // We don't want to retain the default clear screen renderable. Beyond being superfluous, we don't want to serialize it.
   if( renderables.size() == 1 ) {
      ClearScreen* clr = dynamic_cast<ClearScreen*>(renderables[0].get());
      if( clr ) renderables.clear();
   }
   
   renderables.push_back( renderable );
}

void Camera::setRenderPass( const std::shared_ptr<IRenderPass>& rp ) {
   renderPass = rp;
}

unsigned int Camera::numRenderables() {
   return renderables.size();
}

std::shared_ptr<IRenderable> Camera::getRenderable( unsigned int indx ) {
   return renderables[indx];
}

void Camera::update() {
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

void Camera::draw() {
   if( dirty ) {
      renderPass->prepare( renderContext );
      dirty = false;
   }
   
   renderPass->begin( renderContext );
   
   for( auto& renderable : renderables ) {
      renderable->prepare( *renderContext );
      renderable->render( *renderPass );
   }
   
   renderPass->end();
}

//////////////////////////////////////////////////////////////////////////////////////////
// SERIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////
template<class Archive> void Camera::load( Archive& ar ) {
   ar >> renderables;
   ar >> motionController;
   
   std::unique_ptr<RenderPassProxy> rp;
   ar >> rp;
   renderPass = IRenderPass::CreateCopy( *rp );
   
   std::unique_ptr<RenderContextProxy> rc;
   ar >> rc;
   renderContext = IRenderContext::Clone( *rc );
}

template<class Archive> void Camera::save( Archive& ar ) const {
   std::cout<<"Saving Camera "<<name<<std::endl;
   
   ar << renderables;
   ar << motionController;
   
   // Force renderPass to be serialized as base class to maintain scene file platform independence.
   std::unique_ptr<RenderPassProxy> rp = std::make_unique<RenderPassProxy>(*renderPass);
   ar << rp;
  
   std::unique_ptr<RenderContextProxy> rc = std::make_unique<RenderContextProxy>(*renderContext);
   ar << rc;
}

namespace boost { namespace serialization {
   template<class Archive> inline void load(Archive& ar, Camera& t, unsigned int version) {
      std::cout<<"Loading Camera"<<std::endl;
      t.load( ar );
   }
   template<class Archive> inline void save(Archive& ar, const Camera& t, unsigned int version) {
      t.save( ar );
   }
   template<class Archive> inline void serialize(Archive& ar, Camera& t, unsigned int version) {
      boost::serialization::split_free(ar, t, version);
   }
}}
