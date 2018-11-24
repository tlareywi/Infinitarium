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
#include <boost/circular_buffer.hpp>

class PyConsoleRedirect {
public:
   PyConsoleRedirect();

   void write( std::string const& str );
   std::string GetOutput();
   
private:
   static boost::circular_buffer<std::string> m_outputs;
};

class PythonInterpreter : public IPythonInterpreter {
public:
   PythonInterpreter();
   virtual ~PythonInterpreter();
   
   std::string eval( const std::string& ) override;
   
   void setScene( std::shared_ptr<Scene>& );

private:
   boost::python::object main_module;
   boost::python::object main_namespace;
};



