//
//  SceneObject.cpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 3/18/19.
//

#include "SceneObject.hpp"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT(SceneObject)

void SceneObject::addChild( const std::shared_ptr<SceneObject>& child ) {
   children.push_back( child );
}

void SceneObject::removeChild( unsigned int indx ) {
   children.erase( children.begin() + indx );
}

unsigned int SceneObject::numChildren() {
   return children.size();
}

std::shared_ptr<SceneObject> SceneObject::getChild( unsigned int indx ) {
   return children[indx];
}

void SceneObject::prepare( IRenderContext& c ) {
   for( auto& child : children )
      child->prepare( c );
}

void SceneObject::update( UpdateParams& params ) {
   for( auto& child : children )
      child->update( params );
}

void SceneObject::render( IRenderPass& r ) {
   for( auto& child : children )
      child->render( r );
}

namespace boost { namespace serialization {
   template<class Archive> inline void serialize(Archive& ar, SceneObject& t, unsigned int version) {
      std::cout<<"Serializing SceneObject"<<std::endl;
      ar & t.children;
   }
}}
