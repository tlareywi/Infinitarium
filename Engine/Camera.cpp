//
//  Camera.cpp
//  IEPlatform
//
//  Created by Trystan (Home) on 2/20/19.
//

#include "Camera.hpp"

Camera::Camera() : motionController(nullptr) {
   
}

void Camera::setMotionController( std::shared_ptr<IMotionController>& ctrl ) {
   motionController = ctrl;
}

void Camera::setRenderContext( std::shared_ptr<IRenderContext>& r ) {
   renderContext = r;
}

void Camera::add( const std::shared_ptr<IRenderable>& renderable ) {
   std::lock_guard<std::mutex> lock( loadLock );
   
   // We don't want to retain the default clear screen renderable. Beyond being superfluous, we don't want to serialize it.
   if( renderables.size() == 1 ) {
      ClearScreen* clr = dynamic_cast<ClearScreen*>(renderables[0].get());
      if( clr ) renderables.clear();
   }
   
   renderables.push_back( renderable );
}

unsigned int Camera::numRenderables() {
   return renderables.size();
}

std::shared_ptr<IRenderable> Camera::getRenderable( unsigned int indx ) {
   return renderables[indx];
}

void Camera::Draw() {
   renderPass->renderContext = renderContext;
   
   renderPass->begin();
   
   for( auto& renderable : renderables ) {
      renderable->render( *renderPass );
   }
   
   renderPass->end();
}
