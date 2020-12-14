//
//  PythonBridge.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 11/18/18.
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



