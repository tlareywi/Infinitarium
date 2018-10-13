//
//  PointCloud.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#pragma once

#include <vector>

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
   
private:
   std::vector<DataPackContainer> samplers;
   
   std::unique_ptr<IPointInstance> instanceMgr;
};
