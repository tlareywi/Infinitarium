//
//  SimulationWindow.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/8/18.
//

#include "ApplicationWindow.hpp"
#include "Module.hpp"

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(ApplicationWindowProxy)

std::shared_ptr<IApplicationWindow> IApplicationWindow::Create() {
   return ModuleFactory<PlatformFactory>::Instance()->createApplicationWindow();
}

std::shared_ptr<IApplicationWindow> IApplicationWindow::Clone( const IApplicationWindow& obj ) {
   return ModuleFactory<PlatformFactory>::Instance()->cloneApplicationWindow( obj );
}

namespace boost { namespace serialization {
   template<class Archive> inline void serialize(Archive& ar, ApplicationWindowProxy& t, const unsigned int version) {
      std::cout<<"Serializing ApplicationWindowProxy"<<std::endl;
   }
}}


