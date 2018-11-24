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

#include <boost/serialization/map.hpp>

///
/// \brief Handles a 'complex' instance of a point cloud item. Base class can implement default behavior for instance lifetime.
///
class IPointInstance {
   
private:
   std::vector<std::unique_ptr<IPointInstance>> instances;
};

///
/// \brief Collection of renderable points with arbirtary properties defined by buffers/DataPacks.
///
class PointCloud : public IRenderable {
public:
   PointCloud();
   PointCloud( const PointCloud& ) {
      
   }
   
   virtual ~PointCloud() {}
   
   void prepare( IRenderContext& ) override;
   void render( IRenderPass& ) override;
   
   virtual void info() const override;
   
   void addVertexBuffer( DataPackContainer&, const std::string& name );
   
private:
   std::unique_ptr<IPointInstance> instanceMgr;
   
   std::map<std::string, DataPackContainer> vertexBuffers;
   
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version)
   {
      std::cout<<"Serializing Pointcloud"<<std::endl;
      boost::serialization::void_cast_register<PointCloud,IRenderable>();
      boost::serialization::base_object<IRenderable>(*this);
      ar & vertexBuffers;
   }
};

