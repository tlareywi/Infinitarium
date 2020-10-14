//
//  SimulationWindow.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/8/18.
//

#include "ApplicationWindow.hpp"
#include "Module.hpp"

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_IMPLEMENT(ApplicationWindowProxy)

std::shared_ptr<IApplicationWindow> IApplicationWindow::Create() {
   return ModuleFactory<PlatformFactory>::Instance()->createApplicationWindow();
}

std::shared_ptr<IApplicationWindow> IApplicationWindow::Clone( const IApplicationWindow& obj ) {
   return ModuleFactory<PlatformFactory>::Instance()->cloneApplicationWindow( obj );
}

template<class Archive> void ApplicationWindowProxy::serialize(Archive& ar) {
	std::cout << "Serializing ApplicationWindowProxy" << std::endl;
}

namespace boost { namespace serialization {
   template<class Archive> inline void serialize(Archive& ar, ApplicationWindowProxy& t, const unsigned int version) {
      std::cout << "Serializing ApplicationWindowProxy" << std::endl;
	   t.serialize( ar );
   }
}}


