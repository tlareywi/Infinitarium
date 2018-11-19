//
//  PythonBridge.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 11/18/18.
//

#pragma once

#include "EngineInterface.hpp"
#include <boost/python.hpp>

class PythonInterpreter : public IPythonInterpreter {
public:
   PythonInterpreter();
   virtual ~PythonInterpreter();
   
   std::string eval( const std::string& ) override;

private:
   boost::python::object main_module;
   boost::python::object main_namespace;
};



