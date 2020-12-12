//
//  Scene.hpp
//  Infinitaruim-Engine
//
//  Created by Trystan Larey-Williams on 10/13/18.
//

#pragma once

#include <string>
#include <vector>

#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "Camera.hpp"
#include "ConsoleInterface.hpp"

#include "../config.h"

class IE_EXPORT Scene : public SceneObject, public Reflection::IConsole<Scene> {
public:
   Scene();
   virtual ~Scene() {}
   void save( const std::string& ) const;
   void load( const std::string& );
   void loadLocal( const std::string& );
   void setLocalScenePath( const std::string& );
   void clear();
   void update();
   void render();
   void add( const std::shared_ptr<Camera>& );
   
   // SceneObject ////////////////////////////////////
   void prepare(IRenderContext&) override {};
   void update(UpdateParams&) override {};
   void render(IRenderPass&) override {};
   void visit(const class Visitor&) override;
   void visit(const class Accumulator&) override;
   
   auto reflect() {  // IConsole /////////////////////
      static auto tup = make_tuple(
         REFLECT_METHOD(&Scene::save, save),
         REFLECT_METHOD(&Scene::load, load),
         REFLECT_METHOD(&Scene::clear, clear)
      );
      return tup;
   }
   
private:
   std::vector<std::shared_ptr<Camera>> cameras;
   std::vector<std::shared_ptr<IRenderContext>> renderContexts; // Owned by cameras but we need runtime references to notify begin/end frame
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version);
   
   std::mutex loadLock;
   
   // This object is non-copyable. Can be indirectly copied easilly by saving/loading new instance. 
   Scene( const Scene& ) = delete;
   Scene& operator=( const Scene& ) = delete;
   
   std::string localScenePath;
};

BOOST_CLASS_EXPORT_KEY(Scene);
