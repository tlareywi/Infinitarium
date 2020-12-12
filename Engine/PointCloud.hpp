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
#include "MotionControllerOrbit.hpp"

#include "../config.h"

#include <boost/serialization/map.hpp>
#include <boost/serialization/variant.hpp>

#include <chrono>

///
/// \brief Handles a 'complex' instance of a point cloud item. Base class can implement default behavior for instance lifetime.
///
class IPointInstance : public IRenderable {
public:
   enum Type {
      Star,
      Asteroid
   };
   
   void update( UpdateParams& ) override;
   
protected:
   IPointInstance() {}
   std::vector<std::unique_ptr<IPointInstance>> instances;
   
private:
   std::chrono::high_resolution_clock::time_point birth;
};

///
/// \brief Calculations and rendering specific to an instance of a star.
///
class Star : public IPointInstance {
public:
   Star( const glm::vec3&, const glm::vec3&, float );
   
   void prepare( IRenderContext& ) override;
   void render( IRenderPass& ) override;
};

///
/// \brief Collection of renderable points with arbirtary properties defined by buffers/DataPacks.
///
class PointCloud : public IRenderable {
public:
   PointCloud();
   PointCloud( const PointCloud& obj ) : IRenderable( obj ) {
   }
   
   virtual ~PointCloud() {
      vertexBuffers.clear();
   }
   
   void prepare( IRenderContext& ) override;
   void update( UpdateParams& ) override;
   void render( IRenderPass& ) override;
   
   void addVertexBuffer( DataPackContainer&, const std::string& name );
   
   void setNumPoints( unsigned int n ) {
      numPoints = n;
   }
   
private:
   glm::uvec2 pickCoords;
   std::unique_ptr<IPointInstance> instanceMgr;
   std::shared_ptr<IMotionController> motionController;
   
   std::map<std::string, DataPackContainer> vertexBuffers;
   unsigned int numPoints;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      std::cout<<"Serializing Pointcloud"<<std::endl;
      boost::serialization::void_cast_register<PointCloud,IRenderable>();
      ar & boost::serialization::make_nvp("IRenderable", boost::serialization::base_object<IRenderable>(*this));
      ar & BOOST_SERIALIZATION_NVP(vertexBuffers);
      ar & BOOST_SERIALIZATION_NVP(numPoints);
   }
};

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY(PointCloud);


