//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "EngineInterface.hpp"
#include "Scene.hpp"

#include <boost/python.hpp>

class IE_EXPORT PythonInterpreter : public IPythonInterpreter {
public:
   PythonInterpreter();
   virtual ~PythonInterpreter();
   
   std::string eval( const std::string& ) override;
   
   void setScene( std::shared_ptr<Scene>& );

private:
   std::shared_ptr<boost::python::object> main_module;
   std::shared_ptr < boost::python::object> main_namespace;
};



