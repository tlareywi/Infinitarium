//
//  PythonBridge.cpp
//  Infinitaruim-Engine
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#include "DataPack.hpp"
#include "PointCloud.hpp"
#include "Scene.hpp"
#include "PythonBridge.hpp"

#include <boost/python.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::python;

void (DataPack_FLOAT32::*addVec3f)(float,float,float) = &DataPack_FLOAT32::addVec;
void (DataPack_FLOAT32::*addVec4f)(float,float,float,float) = &DataPack_FLOAT32::addVec;
void (DataPack_UINT16::*addVec3u16)(uint16_t,uint16_t,uint16_t) = &DataPack_UINT16::addVec;
void (DataPack_UINT16::*addVec4u16)(uint16_t,uint16_t,uint16_t,uint16_t) = &DataPack_UINT16::addVec;
void (DataPack_UINT32::*addVec3u32)(uint32_t,uint32_t,uint32_t) = &DataPack_UINT32::addVec;
void (DataPack_UINT32::*addVec4u32)(uint32_t,uint32_t,uint32_t,uint32_t) = &DataPack_UINT32::addVec;

BOOST_PYTHON_MODULE(InfinitariumEngine)
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
   
   class_<IRenderable, boost::noncopyable>("IRenderable", no_init)
   ;
   class_<PointCloud, bases<IRenderable>>("PointCloud", init<>())
      .def("addVertexBuffer", &PointCloud::addVertexBuffer)
   ;
   
   class_<Scene>("Scene", init<>())
      .def("load", &Scene::load)
      .def("save", &Scene::save)
      .def("add", &Scene::add)
   ;
   
   register_ptr_to_python<std::shared_ptr<IRenderable>>();
}

PythonInterpreter::PythonInterpreter() {
   Py_Initialize();
   main_module = import("__main__");
   main_namespace = main_module.attr("__dict__");
}

PythonInterpreter::~PythonInterpreter() {
   Py_Finalize();
}
   
std::string PythonInterpreter::eval( const std::string& expr ) {
   try {
      object result = ::eval( str(expr), main_namespace);
      double val = extract<double>(result);
      return boost::lexical_cast<std::string>(val);
   }
   catch( error_already_set const & ) {
      PyObject *ptype, *pvalue, *ptraceback;
      PyErr_Fetch(&ptype, &pvalue, &ptraceback);
      
      handle<> hType(ptype);
      object extype(hType);
      handle<> hTraceback(ptraceback);
      object traceback(hTraceback);
      
      //Extract error message
      std::string strErrorMessage = extract<std::string>(pvalue);
      
      //Extract line number (top entry of call stack)
      // if you want to extract another levels of call stack
      // also process traceback.attr("tb_next") recurently
      
      //long lineno = extract<long> (traceback.attr("tb_lineno"));
      //std::string filename = extract<std::string>(traceback.attr("tb_frame").attr("f_code").attr("co_filename"));
      //std::string funcname = extract<std::string>(traceback.attr("tb_frame").attr("f_code").attr("co_name"));
      
      // For now, just return top level error
      return strErrorMessage;
   }
}


