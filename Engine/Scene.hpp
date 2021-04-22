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

using ReferenceTime = std::chrono::duration<double, std::milli>;

class IE_EXPORT Scene : public SceneObject, public Reflection::IConsole<Scene> {
public:
   Scene();
   virtual ~Scene();

   void save( const std::string& ) const;
   void prepareLoadScene(const std::string&);
   void setLocalScenePath( const std::string& );
   void load(const std::string&);
   void update( const ReferenceTime& );
   void render();
   void add( const std::shared_ptr<Camera>& );
   void terminatePending();
   bool isTerminatePending();
   void waitOnIdle();
   void mainThread();
   const ReferenceTime& referenceTime() const {
       return _referenceTime;
   }
   const ReferenceTime& tickTime() const {
       return _tickTime;
   }
   
   // SceneObject ////////////////////////////////////
   void prepare(IRenderContext&) override {};
   void update(UpdateParams&) override {};
   void render(IRenderPass&) override {};
   void visit(const class Visitor&) override;
   void visit(const class Accumulator&) override;
   
   auto reflect() {  // IConsole /////////////////////
      static auto tup = make_tuple(
         REFLECT_METHOD(&Scene::save, save),
         REFLECT_METHOD(&Scene::prepareLoadScene, prepareLoadScene)
      );
      return tup;
   }
   
private:
   void clear();

   std::vector<std::shared_ptr<Camera>> cameras;
   std::vector<std::shared_ptr<IRenderContext>> renderContexts; // Owned by cameras but we need runtime references to notify begin/end frame
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version);
   
   std::atomic<bool> shouldExit{ false };
   std::atomic<bool> canLoad{ true };

   std::atomic<bool> loadPending{ false };
   std::string loadPendingFilename;
   std::mutex loadSceneMutex;
   
   // This object is non-copyable. Can be indirectly copied easilly by saving/loading new instance. 
   Scene( const Scene& ) = delete;
   Scene& operator=( const Scene& ) = delete;
   
   std::string localScenePath;

   ReferenceTime _referenceTime; // Cumulative time app has been running (ms)
   ReferenceTime _tickTime; // Time since last update (ms)

};

BOOST_CLASS_EXPORT_KEY(Scene);
