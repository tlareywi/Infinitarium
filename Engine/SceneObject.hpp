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

#include <boost/archive/polymorphic_xml_woarchive.hpp>
#include <boost/archive/polymorphic_xml_wiarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

class Camera;

class UpdateParams {
public:
   UpdateParams( Camera& c ) : camera( c ) {}
   UpdateParams( const glm::mat4& p, const glm::mat4& v, Camera& c) : camera(c), projection( p ), view( v ), model( glm::mat4(1.0) ) {
   }
   UpdateParams( const UpdateParams& p, const glm::mat4& v, const glm::mat4& m ) : camera(p.camera) {
      projection = p.projection;
      view = v;
      model = m;
   }
   
   glm::mat4 getProjection() {
      return projection;
   }
   
   glm::mat4 getView() {
      return view;
   }
   
   glm::mat4 getModel() {
      return model;
   }
   
   glm::mat4 getMVP() {
      return projection * view * model;
   }
   
   void addModel( const glm::mat4& m ) {
      model = m * model;
   }
   
   Camera& getCamera() {
      return camera;
   }
   
private:
   Camera& camera;
   glm::mat4 projection;
   glm::mat4 view;
   glm::mat4 model;
};

class SceneObject {
public:
   SceneObject() {}
   virtual ~SceneObject() {}
   virtual void prepare( IRenderContext& );
   virtual void update( UpdateParams& );
   virtual void render( IRenderPass& );
   
   virtual glm::vec3 getCenter() { return {0,0,0}; }
   
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

BOOST_CLASS_EXPORT_KEY(SceneObject)
