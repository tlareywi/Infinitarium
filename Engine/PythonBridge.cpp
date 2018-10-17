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


BOOST_PYTHON_MODULE(InfinitariumEngine)
{
   class_<DataPack_FLOAT32>("DataPack_FLOAT32", init<unsigned int>())
      .def("container", &DataPack_FLOAT32::operator DataPackContainer&, return_internal_reference<>())
   ;
   class_<DataPack_UINT16>("DataPack_UINT16", init<unsigned int>())
      .def("container", &DataPack_UINT16::operator DataPackContainer&, return_internal_reference<>())
   ;
   class_<DataPack_UINT32>("DataPack_UINT32", init<unsigned int>())
      .def("container", &DataPack_UINT32::operator DataPackContainer&, return_internal_reference<>())
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


