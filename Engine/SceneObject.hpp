//
//  SceneObject.hpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 3/18/19.
//

#pragma once

#include "RenderContext.hpp"
#include "RenderPass.hpp"

#include <vector>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

class SceneObject {
public:
   SceneObject() {}
   virtual ~SceneObject() {}
   virtual void prepare( IRenderContext& );
   virtual void update( const glm::mat4& );
   virtual void render( IRenderPass& );
   
   void addChild( const std::shared_ptr<SceneObject>& );
   void removeChild( unsigned int );
   size_t numChildren();
   std::shared_ptr<SceneObject> getChild( unsigned int indx );
   
   void setName( const std::string& n ) {
      name = n;
   }
   
private:
   std::string name;
   std::vector<std::shared_ptr<SceneObject>> children;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive&, const unsigned int);
};
