//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "RenderContext.hpp"
#include "RenderPass.hpp"

#include <vector>

#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

class Camera;
class Scene;

/// <summary>
/// State accumulator for update traversal.
/// </summary>
class UpdateParams {
public:
   UpdateParams( Camera& c, Scene& s ) : 
       camera( c ), 
       scene( s ), 
       model(glm::mat4(1.0)), 
       view(glm::mat4(1.0)), 
       projection(glm::mat4(1.0)) {
   }

   UpdateParams( const UpdateParams& obj, const glm::mat4& p, const glm::mat4& v, const glm::mat4& m) : 
       camera(obj.camera), 
       scene(obj.scene),
       projection( p ), 
       view( v ), 
       model( m ) {
   }

   UpdateParams(const UpdateParams& obj) : 
       camera(obj.camera), 
       scene(obj.scene), 
       projection(obj.projection), 
       view(obj.view),
       model(obj.model) {
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

   Scene& getScene() {
       return scene;
   }
   
private:
   Camera& camera;
   Scene& scene;
   glm::mat4 projection;
   glm::mat4 view;
   glm::mat4 model;
};

/// <summary>
/// Base class for all scenegraph nodes. Implements common traversal logic.
/// </summary>
class IE_EXPORT SceneObject : public std::enable_shared_from_this<SceneObject> {
public:
   SceneObject() : dirty(true), name("Undefined") {}
   virtual ~SceneObject() {
       children.clear();
   }

   virtual void prepare( IRenderContext& );
   virtual void update( UpdateParams& );
   virtual void render( IRenderPass& );
   
   virtual void visit(const class Visitor& v);
   virtual void visit(const class Accumulator& v);
   
   virtual glm::vec3 getCenter() { return localPos; }
   
   void addChild( const std::shared_ptr<SceneObject>& );
   void removeChild( unsigned int );
   size_t numChildren();
   std::shared_ptr<SceneObject> getChild( unsigned int indx );
   
   void setName( const std::string& n ) {
      name = n;
   }
   const std::string& getName() const {
       return name;
   }

   void setDirty() {
       dirty = true;
   }
    
   void select() {
       pendingSelect = true;
   }
   void lookAt( float /* duration */ ) {
       pendingLookAt = true;
   }
   void track() {
       pendingTrack = true;
   }
   void tether() {
       pendingTether = true;
   }

protected:
   bool dirty{ true };
   bool pendingSelect{ false };
   bool pendingLookAt{ false };
   bool pendingTrack{ false };
   bool pendingTether{ false };
   
private:
   std::string name;
   glm::dvec3 localPos{ 0.0, 0.0, 0.0 };
   std::vector<std::shared_ptr<SceneObject>> children;

   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive&, const unsigned int);
};

/// <summary>
/// Visitor pattern to apply a function to all scene nodes or sub-graph.
/// </summary>
class Visitor {
public:
    Visitor(std::function<bool(SceneObject&)>& f) : fun{ f } {}
    bool apply(SceneObject& obj) const {
        return fun(obj); // Returning true indecates continuing traversal
    }

private:
    std::function<bool(SceneObject&)> fun;
};

/// <summary>
/// Similar to a visitor pattern but visits every node twice, once on downward traverse and once on return upward.
/// </summary>
class Accumulator {
public:
    Accumulator(std::function<bool(SceneObject&)>& _push, std::function<void(SceneObject&)>& _pop) : pu{ _push }, po{ _pop } {
    }
    bool push(SceneObject& obj) const {
        return pu(obj);
    }
    void pop(SceneObject& obj) const {
        po(obj);
    }

private:
    std::function<bool(SceneObject&)> pu;
    std::function<void(SceneObject&)> po;
};


BOOST_CLASS_EXPORT_KEY(SceneObject)
