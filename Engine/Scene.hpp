//
//  Scene.hpp
//  Infinitaruim-Engine
//
//  Created by Trystan Larey-Williams on 10/13/18.
//

#pragma once

#include <string>
#include <vector>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "Camera.hpp"
#include "ConsoleInterface.hpp"

class Scene : public Reflection::IConsole<Scene> {
public:
   Scene();
   virtual ~Scene() {}
   void save( const std::string& ) const;
   void load( const std::string& );
   void loadLocal( const std::string& );
   void setLocalScenePath( const std::string& );
   void clear();
   
   void add( const std::shared_ptr<Camera>& );
   
   void update();
   void draw();
   
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
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version);
   
   std::mutex loadLock;
   
   // This object is non-copyable. Can be indirectly copied easilly by saving/loading new instance. 
   Scene( const Scene& ) = delete;
   Scene& operator=( const Scene& ) = delete;
   
   std::string localScenePath;
};
