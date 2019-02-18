//
//  PythonBridge.cpp
//  Infinitaruim-Engine
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#include "../config.h"
#include "DataPack.hpp"
#include "PointCloud.hpp"
#include "Scene.hpp"
#include "ConsoleInterface.hpp"
#include "PythonBridge.hpp"
#include "PyUtil.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

using namespace boost::python;

void (DataPack_FLOAT32::*addVec3f)(float,float,float) = &DataPack_FLOAT32::addVec;
void (DataPack_FLOAT32::*addVec4f)(float,float,float,float) = &DataPack_FLOAT32::addVec;
void (DataPack_UINT16::*addVec3u16)(uint16_t,uint16_t,uint16_t) = &DataPack_UINT16::addVec;
void (DataPack_UINT16::*addVec4u16)(uint16_t,uint16_t,uint16_t,uint16_t) = &DataPack_UINT16::addVec;
void (DataPack_UINT32::*addVec3u32)(uint32_t,uint32_t,uint32_t) = &DataPack_UINT32::addVec;
void (DataPack_UINT32::*addVec4u32)(uint32_t,uint32_t,uint32_t,uint32_t) = &DataPack_UINT32::addVec;

BOOST_PYTHON_MODULE(libInfinitariumEngine)
{
   class_<DataPack_FLOAT32>("DataPack_FLOAT32", init<unsigned int>())
      .def("container", &DataPack_FLOAT32::operator DataPackContainer&, return_internal_reference<>())
      .def("add", &DataPack_FLOAT32::add)
      .def("addVec3", addVec3f)
      .def("addVec4", addVec3f)
   ;
   class_<DataPack_UINT16>("DataPack_UINT16", init<unsigned int>())
      .def("container", &DataPack_UINT16::operator DataPackContainer&, return_internal_reference<>())
      .def("add", &DataPack_UINT16::add)
      .def("addVec3", addVec3u16)
      .def("addVec4", addVec3u16)
   ;
   class_<DataPack_UINT32>("DataPack_UINT32", init<unsigned int>())
      .def("container", &DataPack_UINT32::operator DataPackContainer&, return_internal_reference<>())
      .def("add", &DataPack_UINT32::add)
      .def("addVec3", addVec3u32)
      .def("addVec4", addVec3u32)
   ;
   
   class_<DataPackContainer, boost::noncopyable>("DataPackContainer", no_init)
   ;
   
   /* import ctypes
   >>> ctypes.c_ulong(-1)  # stuff Python's -1 into a C unsigned long
   c_ulong(4294967295L)
   >>> _.value
   4294967295L */
   
   class_<UniformType>("UniformType", init<float>())
      .def(init<int>())
      .def(init<unsigned int>())
   ;
   
   class_<IRenderable, boost::noncopyable>("IRenderable", no_init)
      .def("propList", &IRenderable::propList)
      .def("listUniforms", &IRenderable::listUniforms)
      .def("setProgram", &IRenderable::setProgram)
      .def("setUniform", &IRenderable::setUniform)
      .def("setDirty", &IRenderable::setDirty)
      .def("removeUniform", &IRenderable::removeUniform)
      .def("manipulateUniform", &IRenderable::manipulateUniform)
   ;
   class_<PointCloud, bases<IRenderable>>("PointCloud", init<>())
      .def("addVertexBuffer", &PointCloud::addVertexBuffer)
      .def("setNumPoints", &PointCloud::setNumPoints)
   ;
   
   class_<Scene, boost::noncopyable>("Scene", init<>())
      .def("load", &Scene::load)
      .def("save", &Scene::save)
      .def("add", &Scene::add)
      .def("clear", &Scene::clear)
      .def("propList", &Scene::propList)
      .def("numRenderables", &Scene::numRenderables)
      .def("getRenderable", &Scene::getRenderable)
   ;
   
   register_ptr_to_python<std::shared_ptr<IRenderable>>();
   register_ptr_to_python<std::shared_ptr<Scene>>();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PyConsoleRedirect
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
boost::circular_buffer<std::string> PyConsoleRedirect::m_outputs;
   
PyConsoleRedirect::PyConsoleRedirect() {
   m_outputs.set_capacity(100);
}
   
void PyConsoleRedirect::write( std::string const& str ) {
   m_outputs.push_back(str);
}

std::string PyConsoleRedirect::GetOutput() {
   std::stringstream ss;
      
   for( auto& item : m_outputs )
      ss << item;
   
   m_outputs.clear();
   std::string out( ss.str() );
   boost::replace_all(out, "\n", "\\n");
   boost::replace_all(out, "\"", "\\\"");
   
   return out;
}

static PyConsoleRedirect python_stdio_redirector;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PythonInterpreter
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

PythonInterpreter::PythonInterpreter() {
   Py_Initialize();
   main_module = import("__main__");
   main_namespace = main_module.attr("__dict__");
   
   main_namespace["PyConsoleRedirect"] = class_<PyConsoleRedirect>("PyConsoleRedirect", init<>())
      .def("write", &PyConsoleRedirect::write);
   
   import("sys").attr("stderr") = python_stdio_redirector;
   import("sys").attr("stdout") = python_stdio_redirector;
   
   std::string enginePath {INSTALL_ROOT};
   enginePath += "/lib";
   eval("import sys");
   eval(std::string("sys.path.append('") + enginePath + "')");
   eval("import libInfinitariumEngine");
   eval("engine = libInfinitariumEngine");
}

PythonInterpreter::~PythonInterpreter() {
   Py_Finalize();
}
   
std::string PythonInterpreter::eval( const std::string& expr ) {
   std::string retVal {""};
   std::string inStr {expr};
   
   boost::replace_all(inStr, "‘", "'");
   
   try {
      object result = exec( str(inStr), main_namespace );
      
      retVal = python_stdio_redirector.GetOutput();
      
      // May eventually want expression evaluation support, i.e. 1+1, which currently won't work.
  /*    boost::python::extract<double> num(result);
      if( num.check() ) {
         double val = extract<double>(result);
         retVal += boost::lexical_cast<std::string>(val);
      }
      
      boost::python::extract<std::string> str(result);
      if( str.check() ) {
         retVal += extract<std::string>(result);
      } */
   }
   catch( boost::python::error_already_set const& ) {
      PyObject *ptype, *pvalue, *ptraceback;
      PyErr_Fetch(&ptype, &pvalue, &ptraceback);
      
      if( pvalue ) {
         handle<> h_val(pvalue);
         str a(h_val);
         extract<std::string> returned(a);
         if( returned.check() )
            retVal +=  ": " + returned();
         else
            retVal += std::string(": Unparseable Python error: ");
      }
      
      if( ptraceback ) {
         handle<> hTraceback(ptraceback);
         object tb( import("traceback") );
         object fmt_tb(tb.attr("format_tb"));
         // Call format_tb to get a list of traceback strings
         object tb_list(fmt_tb(hTraceback));
         // Join the traceback strings into a single string
         object tb_str(str("\n\\").join(tb_list));
         // Extract the string, check the extraction, and fallback in necessary
         extract<std::string> returned(tb_str);
         if(returned.check())
            retVal += ": " + returned();
         else
            retVal += std::string(": Unparseable Python traceback");
      }
      
      boost::replace_all(retVal, "\n", "\\n");
      boost::replace_all(retVal, "\"", "\\\"");
   }
   catch( ... ) {
      retVal = "PythonInterpreter::eval encountered unexpected exception.";
   }
   
   return retVal;
}

void PythonInterpreter::setScene( std::shared_ptr<Scene>& scene ) {
   boost::python::import("__main__").attr("activeScene") = scene;
}


