//
//  PointCloud.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#pragma once

#include <map>

#include "DataPack.hpp"
#include "Renderable.hpp"

class IPointInstance {
   // TODO: Handles a 'complex' instance of a point cloud item. Base class can implement default behavior for instance lifetime.
   
private:
   std::vector<std::unique_ptr<IPointInstance>> instances;
};

class PointCloud : public IRenderable {
public:
   PointCloud() {}
   PointCloud( const PointCloud& ) {
      
   }
   
   void addVertexBuffer( const DataPackContainer&, const std::string& name );
   
private:
   std::map<std::string, DataPackContainer> vertexBuffers;
   std::unique_ptr<IPointInstance> instanceMgr;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version)
   {
      ar & vertexBuffers;
   }
};
