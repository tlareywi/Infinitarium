//
//  PointCloud.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#include "PointCloud.hpp"

void PointCloud::addVertexBuffer( const DataPackContainer& datapack, const std::string& name ) {
   std::visit( [this, name](auto const& e) {
      vertexBuffers.emplace( std::make_pair(name, std::move(e)) );
   }, datapack );
}


