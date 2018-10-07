//
//  PointCloud.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#pragma once

#include <vector>


class IPointInstance {
   // TODO: Handles a 'complex' instance of a point cloud item. Base class can implement default behavior for instance lifetime.
   
private:
   std::vector<std::unique_ptr<IPointInstance>> instances;
};

class PointCloud {
public:
   PointCloud() {}
   PointCloud( const PointCloud& ) {
      
   }
   
private:
   
   
   std::unique_ptr<IPointInstance> instanceMgr;
};
