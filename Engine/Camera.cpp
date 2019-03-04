//
//  Camera.cpp
//  IEPlatform
//
//  Created by Trystan (Home) on 2/20/19.
//

#include "Camera.hpp"

Camera::Camera() : dirty(false), motionController(nullptr) {
   addRenderable( std::make_shared<ClearScreen>() );
   projection = glm::perspective( glm::radians(60.0), 16.0 / 9.0, 0.0001, 100.0 );
}

void Camera::setMotionController( std::shared_ptr<IMotionController>& ctrl ) {
   motionController = ctrl;
}

void Camera::setRenderContext( std::shared_ptr<IRenderContext>& r ) {
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
