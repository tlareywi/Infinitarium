//
//  Renderable.cpp
//  Infinitarium
//
//  Created by Trystan (Home) on 10/11/18.
//

#include "Renderable.hpp"

#include <boost/serialization/export.hpp>

BOOST_SERIALIZATION_ASSUME_ABSTRACT(IRenderable)
BOOST_CLASS_EXPORT_GUID(IRenderable, "IRenderable")

IRenderable::IRenderable() : dirty(true) {
   pipelineState = IPipelineState::Create();
   renderCommand = IRenderCommand::Create();
}
