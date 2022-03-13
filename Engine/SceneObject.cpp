//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
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
   if (!v.apply(*this)) 
      return;
   
   for (auto& child : children)
      child->visit(v);
}

void SceneObject::visit(const Accumulator& v) {
    if (!v.push(*this))
        return;

    for (auto& child : children)
        child->visit(v);

    v.pop(*this);
}

template<class Archive> void SceneObject::serialize( Archive& ar, const unsigned int version ) {
	std::cout << "Serializing SceneObject" << std::endl;
    ar & BOOST_SERIALIZATION_NVP(name);
	ar & BOOST_SERIALIZATION_NVP(children);
}

