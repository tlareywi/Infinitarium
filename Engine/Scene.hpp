//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
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
using SimulationTime = std::chrono::time_point<std::chrono::system_clock>;
using SimulationTimeDelta = std::chrono::duration<double, std::milli>;

/// <summary>
/// Scene is the top level node for the engine. It may have many Cameras, each of which are traversed for each frame. The render method of this class 
/// calls endFrame on the Camera(s) render context, after interating through all cameras, ultimately invoking a 'present' for the current frame.    
/// </summary>
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
    
   // TODO: Maybe both of these down in Calendar?
   double JD() const {
       return _JD;
   }
   const SimulationTime& simulationTime() const {
       return _simulationTime;
   }
   const SimulationTimeDelta& simulationTimeDelta() const {
        return _simulationTimeDelta;
   }
   int timeMultiplier() const {
       return _timeMultiplier;
   }
   void timeMultiplier( int t ) {
        return _timeMultiplier = t;
   }
   ////////////////////////
   
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
   
   int _timeMultiplier{ 1 };
   ReferenceTime _referenceTime;             // Cumulative time app has been running in real time (ms)
   ReferenceTime _tickTime;                  // Time since last update in real time (ms). Always positive.
   SimulationTime _simulationTime;           // Current time in the context of the simulation
   SimulationTimeDelta _simulationTimeDelta; // Difference in simulation time relative to last update. Can be negative.
   double _JD;                               // _simulationTime in units of Julian Days.
};

BOOST_CLASS_EXPORT_KEY(Scene);
