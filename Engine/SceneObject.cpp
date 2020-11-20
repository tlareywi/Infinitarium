//
//  SceneObject.cpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 3/18/19.
//

#include "SceneObject.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(SceneObject)

void SceneObject::addChild( const std::shared_ptr<SceneObject>& child ) {
   children.push_back( child );
}

void SceneObject::removeChild( unsigned int indx ) {
   children.erase( children.begin() + indx );
}

size_t SceneObject::numChildren() {
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

void SceneObject::visit(const Visitor& v) {
   v.apply(*this);

   for (auto& child : children)
      child->visit(v);
}

template<class Archive> void SceneObject::serialize( Archive& ar, const unsigned int version ) {
	std::cout << "Serializing SceneObject" << std::endl;
	ar & BOOST_SERIALIZATION_NVP(children);
}

