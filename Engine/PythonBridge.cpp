//
//  PythonBridge.cpp
//  Infinitaruim-Engine
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#include "DataPack.hpp"
#include "PointCloud.hpp"
#include "Scene.hpp"

#include <boost/python.hpp>
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
      .def("addVec3", addVec3f)
      .def("addVec4", addVec3f)
   ;
   class_<DataPack_UINT16>("DataPack_UINT16", init<unsigned int>())
      .def("container", &DataPack_UINT16::operator DataPackContainer&, return_internal_reference<>())
      .def("addVec3", addVec3u16)
      .def("addVec4", addVec3u16)
   ;
   class_<DataPack_UINT32>("DataPack_UINT32", init<unsigned int>())
      .def("container", &DataPack_UINT32::operator DataPackContainer&, return_internal_reference<>())
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


