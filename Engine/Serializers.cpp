//
//  Serializers.cpp
//  Infinitaruim-Engine
//
//  Created by Trystan Larey-Williams on 10/13/18.
//

#include "DataPack.hpp"
#include "PointCloud.hpp"

namespace boost {
   namespace serialization {
      
      template<class Archive> void serialize( Archive& ar, DataPackContainer& t, const unsigned int version ) {
         std::visit( [&ar](auto const& e){ ar & e; }, t );
      }
   }
}
